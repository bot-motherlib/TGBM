#include "tgbm/net/asio_ssl_connection.hpp"
#include "tgbm/logger.h"
#include "tgbm/net/asio_awaiters.hpp"
#include "tgbm/net/errors.hpp"
#include "tgbm/tools/scope_exit.h"

namespace tgbm {

void asio_ssl_connection::shutdown() noexcept {
  if (!socket.lowest_layer().is_open())
    return;
  io_error_code ec;
  // TODO check if already closed
  // must stop writer if writer on asio queues
  ec = socket.lowest_layer().cancel(ec);
  // dont stop on errors, i need to stop connection somehow
  if (ec)
    LOG_ERR("[HTTP2] [shutdown] TCP socket cancel, err {}", ec.what());
  // Do not do SSL shutdown, because TG does not too, useless errors and wasting time
  ec = socket.lowest_layer().shutdown(asio::socket_base::shutdown_both, ec);
  if (ec)
    LOG_ERR("[HTTP2] [shutdown] TCP socket shutdown, err: {}", ec.what());
  ec = socket.lowest_layer().close(ec);
  if (ec)
    LOG_ERR("[HTTP2] [shutdown], TCP socket close err: {}", ec.what());
}

dd::task<std::shared_ptr<asio_ssl_connection>> asio_ssl_connection::create(asio::io_context& io_ctx,
                                                                           std::string host,
                                                                           asio::ssl::context sslctx) {
  namespace ssl = asio::ssl;
  using tcp = asio::ip::tcp;

  tcp::resolver resolver(io_ctx);
  std::shared_ptr connection = std::make_shared<asio_ssl_connection>(io_ctx, std::move(sslctx));
  ssl::stream<tcp::socket>& socket = connection->socket;
  tcp::resolver::query query(host, "https");
  io_error_code ec;
  auto results = co_await net.resolve(resolver, query, ec);
  if (ec)
    throw network_exception("[http] cannot resolve host: {}: service: {}, err: {}", query.host_name(),
                            query.service_name(), ec.message());
  co_await net.connect(socket.lowest_layer(), results, ec);
  if (ec)
    throw network_exception("[http] cannot connect to {}, err: {}", host, ec.message());
  socket.lowest_layer().set_option(tcp::no_delay(false));
  // TODO подобрать хорошие значения
  socket.lowest_layer().set_option(asio::socket_base::send_buffer_size(32768));
  socket.lowest_layer().set_option(asio::socket_base::receive_buffer_size(32768));
  socket.set_verify_mode(ssl::verify_none);
  socket.set_verify_callback(ssl::host_name_verification(host));

  co_await net.handshake(socket, ssl::stream_base::handshake_type::client, ec);
  if (ec)
    throw network_exception("[http] cannot ssl handshake: {}", ec.message());
  co_return connection;
}

}  // namespace tgbm
