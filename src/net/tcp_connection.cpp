#include <tgbm/net/tcp_connection.hpp>

#include <tgbm/logger.hpp>
#include <tgbm/net/asio_awaiters.hpp>
#include <tgbm/net/errors.hpp>

#include <boost/asio/ssl/host_name_verification.hpp>

namespace tgbm {

// TLS CONNECTION

dd::task<tcp_tls_connection> tcp_tls_connection::create(asio::io_context& io_ctx, std::string host,
                                                        ssl_context_ptr sslctx,
                                                        tcp_connection_options options) {
  namespace ssl = asio::ssl;
  using tcp = asio::ip::tcp;

  tcp::resolver resolver(io_ctx);
  tcp_tls_connection connection(io_ctx, std::move(sslctx));
  ssl::stream<tcp::socket>& socket = connection.socket;

  asio::ip::basic_resolver_query<asio::ip::tcp> query(host, "https");
  io_error_code ec;
  auto results = co_await net.resolve(resolver, query, ec);
  if (results.empty() || ec) {
    TGBM_LOG_ERROR("[TCP] cannot resolve host: {}, err: {}", host, ec.what());
    throw network_exception(ec);
  }
  auto& tcp_sock = socket.lowest_layer();

  co_await net.connect(tcp_sock, std::move(results), ec);

  if (ec) {
    TGBM_LOG_ERROR("[TCP] cannot connect to {}, err: {}", host, ec.message());
    throw network_exception(ec);
  }
  options.apply(tcp_sock);
  socket.set_verify_mode(options.disable_ssl_certificate_verify ? ssl::verify_none : ssl::verify_peer);
  socket.set_verify_callback(ssl::host_name_verification(host));
  if (!options.is_primal_connection)
    SSL_set_mode(socket.native_handle(), SSL_MODE_RELEASE_BUFFERS);
  co_await net.handshake(socket, ssl::stream_base::handshake_type::client, ec);
  if (ec) {
    TGBM_LOG_ERROR("[TCP/SSL] cannot ssl handshake: {}", ec.message());
    throw network_exception(ec);
  }
  co_return connection;
}

void tcp_tls_connection::start_read(std::coroutine_handle<> h, std::span<byte_t> buf, io_error_code& ec) {
  asio::async_read(socket, asio::buffer(buf.data(), buf.size()), [&, h](const io_error_code& e, size_t) {
    ec = e;
    h.resume();
  });
}

void tcp_tls_connection::start_write(std::coroutine_handle<> h, std::span<const byte_t> buf,
                                     io_error_code& ec, size_t& written) {
  asio::async_write(socket, asio::buffer(buf.data(), buf.size()), [&, h](const io_error_code& e, size_t w) {
    written = w;
    ec = e;
    h.resume();
  });
}

dd::task<void> tcp_tls_connection::yield() {
  co_await dd::suspend_and_t{[&](std::coroutine_handle<> h) { asio::post(socket.get_executor(), h); }};
}

void tcp_tls_connection::shutdown() noexcept {
  auto& tcp_sock = socket.lowest_layer();
  if (!tcp_sock.is_open())
    return;
  io_error_code ec;
  ec = tcp_sock.cancel(ec);
  // dont stop on errors, i need to stop connection somehow
  if (ec)
    TGBM_LOG_ERROR("[HTTP2] [shutdown] TCP socket cancel, err {}", ec.what());
  // Do not do SSL shutdown, because TG does not too, useless errors and wasting time
  ec = tcp_sock.shutdown(asio::socket_base::shutdown_both, ec);
  if (ec)
    TGBM_LOG_ERROR("[HTTP2] [shutdown] TCP socket shutdown, err: {}", ec.what());
  ec = tcp_sock.close(ec);
  if (ec)
    TGBM_LOG_ERROR("[HTTP2] [shutdown], TCP socket close err: {}", ec.what());
}

// WITHOUT TLS

dd::task<tcp_connection> tcp_connection::create(asio::io_context& io_ctx, std::string host,
                                                tcp_connection_options options) {
  using tcp = asio::ip::tcp;

  tcp::resolver resolver(io_ctx);
  tcp_connection connection(io_ctx);
  tcp::socket& tcp_sock = connection.socket;
  asio::ip::basic_resolver_query<asio::ip::tcp> query(host, "http");
  io_error_code ec;
  auto results = co_await net.resolve(resolver, query, ec);
  if (ec) {
    TGBM_LOG_ERROR("[TCP] cannot resolve host: {}, err: {}", host, ec.what());
    throw network_exception(ec);
  }
  co_await net.connect(tcp_sock, results, ec);

  if (ec) {
    TGBM_LOG_ERROR("[TCP] cannot connect to {}, err: {}", host, ec.message());
    throw network_exception(ec);
  }
  options.apply(tcp_sock);

  co_return connection;
}

void tcp_connection::start_read(std::coroutine_handle<> h, std::span<byte_t> buf, io_error_code& ec) {
  asio::async_read(socket, asio::buffer(buf.data(), buf.size()), [&, h](const io_error_code& e, size_t) {
    ec = e;
    h.resume();
  });
}

void tcp_connection::start_write(std::coroutine_handle<> h, std::span<const byte_t> buf, io_error_code& ec,
                                 size_t& written) {
  asio::async_write(socket, asio::buffer(buf.data(), buf.size()), [&, h](const io_error_code& e, size_t w) {
    written = w;
    ec = e;
    h.resume();
  });
}

dd::task<void> tcp_connection::yield() {
  co_await dd::suspend_and_t{[&](std::coroutine_handle<> h) { asio::post(socket.get_executor(), h); }};
}

void tcp_connection::shutdown() noexcept {
  auto& tcp_sock = socket.lowest_layer();
  if (!tcp_sock.is_open())
    return;
  io_error_code ec;
  ec = tcp_sock.cancel(ec);
  // dont stop on errors, i need to stop connection somehow
  if (ec)
    TGBM_LOG_ERROR("[HTTP2] [shutdown] TCP socket cancel, err {}", ec.what());
  // Do not do SSL shutdown, because TG does not too, useless errors and wasting time
  ec = tcp_sock.shutdown(asio::socket_base::shutdown_both, ec);
  if (ec)
    TGBM_LOG_ERROR("[HTTP2] [shutdown] TCP socket shutdown, err: {}", ec.what());
  ec = tcp_sock.close(ec);
  if (ec)
    TGBM_LOG_ERROR("[HTTP2] [shutdown], TCP socket close err: {}", ec.what());
}

}  // namespace tgbm
