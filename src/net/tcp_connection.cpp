#include "tgbm/net/tcp_connection.hpp"

#include "tgbm/logger.h"
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
    LOG_ERR("[HTTP2] [shutdown] TCP socket cancel, err {}", ec.what());
  // Do not do SSL shutdown, because TG does not too, useless errors and wasting time
  ec = tcp_sock.shutdown(asio::socket_base::shutdown_both, ec);
  if (ec)
    LOG_ERR("[HTTP2] [shutdown] TCP socket shutdown, err: {}", ec.what());
  ec = tcp_sock.close(ec);
  if (ec)
    LOG_ERR("[HTTP2] [shutdown], TCP socket close err: {}", ec.what());
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
  if (ec)
    throw network_exception("[http] cannot resolve host: {}: service: {}, err: {}", query.host_name(),
                            query.service_name(), ec.message());
  auto& tcp_sock = socket.lowest_layer();
  co_await net.connect(tcp_sock, results, ec);
  if (ec)
    throw network_exception("[http] cannot connect to {}, err: {}", host, ec.message());
  tcp_sock.set_option(tcp::no_delay(!options.merge_small_requests));
  {
    asio::socket_base::send_buffer_size send_sz_option(options.send_buffer_size);
    tcp_sock.set_option(send_sz_option);
    tcp_sock.get_option(send_sz_option);
    if (send_sz_option.value() != options.send_buffer_size) {
      LOG_WARN("tcp sendbuf size option not fully applied, requested: {}, actual: {}",
               options.send_buffer_size, send_sz_option.value());
    }
  }
  {
    asio::socket_base::receive_buffer_size rsv_sz_option(options.receive_buffer_size);
    tcp_sock.set_option(rsv_sz_option);
    tcp_sock.get_option(rsv_sz_option);
    if (rsv_sz_option.value() != options.send_buffer_size) {
      LOG_WARN("tcp receive buf size option not fully applied, requested: {}, actual: {}",
               options.send_buffer_size, rsv_sz_option.value());
    }
  }
  socket.set_verify_mode(options.disable_ssl_certificate_verify ? ssl::verify_none : ssl::verify_peer);
  socket.set_verify_callback(ssl::host_name_verification(host));
  if (!options.is_primal_connection)
    SSL_set_mode(socket.native_handle(), SSL_MODE_RELEASE_BUFFERS);
  co_await net.handshake(socket, ssl::stream_base::handshake_type::client, ec);
  if (ec)
    throw network_exception(
        "[http] cannot ssl handshake: {}"
#ifndef TGBM_SSL_ADDITIONAL_CERTIFICATE_PATH
        ". If your certificate is not default or you are on windows (where default pathes may be unreachable)"
        " define TGBM_SSL_ADDITIONAL_CERTIFICATE_PATH to provide additional certificate or use option "
        "'disable_ssl_certificate_verify' (only for testing)"
#endif
        ,
        ec.message());
  co_return connection;
}

}  // namespace tgbm
