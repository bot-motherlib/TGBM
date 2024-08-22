#include "tgbm/net/boost_http_client.h"

#include <charconv>
#include <cstddef>

#include "tgbm/tools/scope_exit.h"
#include "tgbm/net/asio_awaiters.h"

namespace tgbm {

dd::task<std::shared_ptr<asio_connection_t>> create_connection(
    boost::asio::io_context& io_ctx KELCORO_LIFETIMEBOUND, std::string host) {
  namespace asio = boost::asio;
  namespace ssl = asio::ssl;
  using tcp = asio::ip::tcp;

  tcp::resolver resolver(io_ctx);
  std::shared_ptr connection = [&io_ctx] {
    ssl::context sslctx(ssl::context::tlsv12_client);
    sslctx.set_options(ssl::context::default_workarounds | ssl::context::no_sslv2 |
                       ssl::context::single_dh_use);
    sslctx.set_default_verify_paths();
    return std::make_shared<asio_connection_t>(io_ctx, std::move(sslctx));
  }();
  ssl::stream<tcp::socket>& socket = connection->socket;
  // TODO ? is needed?
  tcp::resolver::query query(host, "https");
  io_error_code ec;
  auto results = co_await async_resolve(resolver, query, ec);
  if (ec)
    throw network_exception("[http] cannot resolve host: {}: service: {}, err: {}", query.host_name(),
                            query.service_name(), ec.message());
  (void)co_await async_connect(socket.lowest_layer(), results, ec);
  if (ec)
    throw network_exception("[http] cannot connect to {}, err: {}", host, ec.message());
  socket.lowest_layer().set_option(tcp::no_delay(true));
  // TODO подобрать хорошие значения
  socket.lowest_layer().set_option(asio::socket_base::send_buffer_size(32768));
  socket.lowest_layer().set_option(asio::socket_base::receive_buffer_size(32768));
  socket.set_verify_mode(ssl::verify_none);
  socket.set_verify_callback(ssl::host_name_verification(host));

  co_await async_handshake(socket, ssl::stream_base::handshake_type::client, ec);
  if (ec)
    throw network_exception("[http] cannot ssl handshake: {}", ec.message());
  co_return connection;
}

dd::task<http_response> send_request(std::shared_ptr<asio_connection_t> con, http_request& request) {
  namespace asio = boost::asio;
  using tcp = asio::ip::tcp;
  assert(con);
  auto& socket = con->socket;

  io_error_code ec;
  auto handle_timeout = [](io_error_code& ec) -> bool {
    if (ec == asio::error::operation_aborted) {
      throw timeout_exception("[send request] timed out");
    }
    return true;  // for chaining
  };
  LOG_DEBUG("generated request:\nHEADERS:\n{}\nBODY:\n{}", request.headers, request.body);
  std::array<asio::const_buffer, 2> headers_and_body{asio::buffer(request.headers),
                                                     asio::buffer(request.body)};
  size_t transfered = co_await async_write(socket, headers_and_body, ec);
  if (ec && handle_timeout(ec))
    throw http_exception("[http] cannot write to {} socket, err: {}", (void*)&socket, ec.message());
  assert(transfered == (request.headers.size() + request.body.size()));
  request.headers.clear();
  // TODO нужно возвращать вектор байт вместо строки 1. это выгоднее (sso не нужно)
  // 2. там могут быть \0, т.к. это в том числе загрузка файлов

  // TODO read http status string first, return http status code too
  // HTTP<version><whitespaces><code>
  static constexpr std::string_view headers_end_marker = "\r\n\r\n";
  // read http headers
  std::string::size_type headers_end_pos =
      co_await async_read_until(socket, request.headers, headers_end_marker, ec);
  if (ec && handle_timeout(ec))
    throw http_exception("[http] error while reading http headers: {}", ec.message());
  static constexpr std::string_view content_length_header = "Content-Length:";
  auto content_len_pos = request.headers.find(content_length_header);
  if (content_len_pos == request.headers.npos)
    throw http_exception("[http] incorrect server answer");
  content_len_pos += content_length_header.size();
  // skip whitespaces (exactly one usually)
  content_len_pos = request.headers.find_first_not_of(' ', content_len_pos);
  assert(content_len_pos < headers_end_pos);
  size_t content_length;
  auto [_, err] = std::from_chars(request.headers.data() + content_len_pos,
                                  request.headers.data() + headers_end_pos, content_length);
  if (err != std::errc{})
    throw http_exception("[http] cannot parse content length from server response: {}",
                         std::make_error_code(err).message());

  headers_end_pos += headers_end_marker.size();
  size_t overriden_bytes = request.headers.size() - headers_end_pos;
  assert(overriden_bytes <= content_length);
  // store overriden body and erase it from headers
  request.body.resize(content_length);
  memcpy(request.body.data(), request.headers.data() + headers_end_pos, overriden_bytes);
  request.headers.erase(request.headers.begin() + headers_end_pos, request.headers.end());
  LOG_DEBUG("response headers:\n{}", request.headers);
  transfered = co_await async_read(
      socket, asio::buffer(request.body.data() + overriden_bytes, content_length - overriden_bytes), ec);
  if (ec && handle_timeout(ec))
    throw http_exception("[http] error while reading body: {}", ec.message());
  assert(transfered == content_length - overriden_bytes);
  LOG_DEBUG("response body:\n{}", request.body);
  // reuses memory
  co_return http_response{.body = std::move(request.body), .headers = std::move(request.headers)};
}

// TODO передавать таймаут в операцию (и удалить тот клиент который не поддерживает)
dd::task<http_response> boost_singlethread_client::send_request(http_request request, duration_t timeout) {
  // TODO borrow connection with timeout
  auto handle = co_await connections.borrow();

  boost::asio::steady_timer timer(io_ctx);
  timer.expires_after(timeout);
  timer.async_wait([socket = &handle.get()->get()->socket](const io_error_code& ec) {
    if (ec == boost::asio::error::operation_aborted)
      return;
    // if im working and ctx on one thread, then now coroutine suspended
    socket->lowest_layer().cancel();
    // coro should be resumed and operation_aborted (coro ended with exception)
  });
  on_scope_failure(drop_connection) {
    handle.drop();
  };
  on_scope_exit {
    timer.cancel();
  };
  http_response rsp = co_await tgbm::send_request(*handle.get(), request);
  drop_connection.no_longer_needed();
  co_return rsp;
}

void boost_singlethread_client::run() {
  // TODO check not invoked already
  if (io_ctx.stopped())
    io_ctx.restart();
  io_ctx.run();
}

bool boost_singlethread_client::run_one(duration_t timeout) {
  // TODO check not invoked already
  if (io_ctx.stopped())
    io_ctx.restart();
  auto count = io_ctx.run_one_for(timeout);
  return count > 0;
}

void boost_singlethread_client::stop() {
  io_ctx.stop();
}

}  // namespace tgbm
