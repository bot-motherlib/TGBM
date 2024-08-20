#include "tgbm/net/BoostHttpOnlySslClient.h"

#include <charconv>
#include <cstddef>

#include "tgbm/scope_exit.h"
#include "tgbm/net/asio_awaiters.h"

namespace tgbm {

BoostHttpOnlySslClient::BoostHttpOnlySslClient(boost::asio::io_context& ctx, std::string host,
                                               size_t connections_max_count)
    : io_ctx(ctx),
      connections(
          connections_max_count, [io = &io_ctx, h = std::move(host)]() { return create_connection(*io, h); },
          exe) {
}

BoostHttpOnlySslClient::~BoostHttpOnlySslClient() = default;

dd::task<BoostHttpOnlySslClient::connection_t> BoostHttpOnlySslClient::create_connection(
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
  asio::connect(socket.lowest_layer(), results, ec);
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

dd::task<std::string> BoostHttpOnlySslClient::makeRequest(http_request request) {
  namespace asio = boost::asio;
  using tcp = asio::ip::tcp;

  // TODO? retry if throw?
  auto handle = co_await connections.borrow();
  auto& socket = handle.get()->get()->socket;
  on_scope_failure(drop_socket) {
    // avoid drop connection on timeout (handle.destroy())
    if (std::uncaught_exceptions())
      handle.drop();
    else
      socket.lowest_layer().cancel();
  };
  io_error_code ec;
  LOG_DEBUG("generated request:\n HEADERS:\n{}\nBODY:\n{}", request.headers, request.body);
  std::array<asio::const_buffer, 2> headers_and_body{asio::buffer(request.headers),
                                                     asio::buffer(request.body)};
  size_t transfered = co_await async_write(socket, headers_and_body, ec);
  if (ec)
    throw http_exception("[http] cannot write to {} socket, err: {}", (void*)&socket, ec.message());
  assert(transfered == (request.headers.size() + request.body.size()));
  // TODO timeout на весь сокет сразу одну штуку(точнее на одну операцию(корутину)...)
  // TODO reuse memory from http request
  std::string data;
  // TODO нужно возвращать вектор байт вместо строки 1. это выгоднее (sso не нужно)
  // 2. там могут быть \0, т.к. это в том числе загрузка файлов

  // TODO read http status string first, return http status code too
  // HTTP<version><whitespaces><code>
  static constexpr std::string_view headers_end_marker = "\r\n\r\n";
  // read http headers
  std::string::size_type headers_end_pos = co_await async_read_until(socket, data, headers_end_marker, ec);
  // TODO получается тут 3 read_until - status string \r\n, Content-Length: (хотя это ... Хм), headers end
  // \r\n\r\n
  if (ec)
    throw http_exception("[http] error while reading http headers: {}", ec.message());
  static constexpr std::string_view content_length_header = "Content-Length:";
  auto content_len_pos = data.find(content_length_header);
  if (content_len_pos == data.npos)
    throw http_exception("[http] incorrect server answer");
  content_len_pos += content_length_header.size();
  // skip whitespaces (exactly one usually)
  content_len_pos = data.find_first_not_of(' ', content_len_pos);
  assert(content_len_pos < headers_end_pos);
  size_t content_length;
  auto [_, err] =
      std::from_chars(data.data() + content_len_pos, data.data() + headers_end_pos, content_length);
  if (err != std::errc{})
    throw http_exception("[http] cannot parse content length from server response: {}",
                         std::make_error_code(err).message());

  headers_end_pos += headers_end_marker.size();
  size_t overriden_bytes = data.size() - headers_end_pos;
  assert(overriden_bytes <= content_length);
  // remove headers, keep only overreaded body
  LOG_DEBUG("response headers:\n{}", std::string_view(data).substr(0, data.size() - overriden_bytes));
  data.erase(data.begin(), data.begin() + headers_end_pos);
  data.resize(content_length);
  transfered = co_await async_read(
      socket, asio::buffer(data.data() + overriden_bytes, content_length - overriden_bytes), ec);
  if (ec)
    throw http_exception("[http] error while reading body: {}", ec.message());
  assert(transfered == content_length - overriden_bytes);
  drop_socket.no_longer_needed();
  LOG_DEBUG("response body:\n{}", data);
  co_return data;
}

}  // namespace tgbm
