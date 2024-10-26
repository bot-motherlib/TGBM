#include "tgbm/net/tcp_connection.hpp"

#include "tgbm/logger.hpp"
#include "tgbm/net/asio_awaiters.hpp"
#include "tgbm/net/errors.hpp"

#include "boost/asio/ssl/host_name_verification.hpp"

namespace tgbm {

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

dd::task<tcp_connection_ptr> tcp_connection::create(asio::io_context& io_ctx, std::string host,
                                                    ssl_context_ptr sslctx, tcp_connection_options options) {
  namespace ssl = asio::ssl;
  using tcp = asio::ip::tcp;

  tcp::resolver resolver(io_ctx);
  tcp_connection_ptr connection = new tcp_connection(io_ctx, std::move(sslctx));
  ssl::stream<tcp::socket>& socket = connection->socket;
  tcp::resolver::query query(host, "https");
  io_error_code ec;
  auto results = co_await net.resolve(resolver, query, ec);
  if (ec) {
    TGBM_LOG_ERROR("[TCP] cannot resolve host: {}: service: {}, err: {}", query.host_name(),
                   query.service_name(), ec.message());
    throw network_exception(ec);
  }
  auto& tcp_sock = socket.lowest_layer();
  co_await net.connect(tcp_sock, results, ec);

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

}  // namespace tgbm
