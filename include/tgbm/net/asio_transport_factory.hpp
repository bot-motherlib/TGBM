#pragma once

#include <boost/asio/io_context.hpp>
#undef NO_ERROR
#undef Yield
#undef min
#undef max
#include <tgbm/net/tcp_connection.hpp>
#include <tgbm/net/asio_awaiters.hpp>
#include <tgbm/net/any_connection.hpp>
#include <tgbm/utils/deadline.hpp>

namespace tgbm {

// TODO into .cpp
struct asio_connection {
  tcp_connection_ptr connection = nullptr;

  void start_read(std::coroutine_handle<> h, std::span<byte_t> buf, io_error_code& ec) {
    assert(connection);
    asio::async_read(connection->socket, asio::buffer(buf.data(), buf.size()),
                     [&, h](const io_error_code& e, size_t) {
                       ec = e;
                       h.resume();
                     });
  }
  void start_write(std::coroutine_handle<> h, std::span<const byte_t> buf, io_error_code& ec,
                   size_t& written) {
    assert(connection);
    asio::async_write(connection->socket, asio::buffer(buf.data(), buf.size()),
                      [&, h](const io_error_code& e, size_t w) {
                        written = w;
                        ec = e;
                        h.resume();
                      });
  }
  void shutdown() noexcept {
    connection->shutdown();
  }
  dd::task<void> yield() {
    co_await dd::suspend_and_t{
        [&](std::coroutine_handle<> h) { asio::post(connection->socket.get_executor(), h); }};
    // TODO co_await on empty 'void' task must return normally
    co_return;
  }
};

struct asio_transport_factory {
 private:
  std::unique_ptr<boost::asio::io_context> io_ctx;
  tcp_connection_options tcp_options;

 public:
  explicit asio_transport_factory(tcp_connection_options opts = {})
      : io_ctx(std::make_unique<boost::asio::io_context>(1)), tcp_options(std::move(opts)) {
    if (tcp_options.disable_ssl_certificate_verify)
      TGBM_LOG_WARN("SSL veriication for http2 client disabled");
  }

  dd::task<any_connection> create_connection(std::string_view host) {
    assert(io_ctx);
    tcp_connection_ptr con = co_await tcp_connection::create(
        *io_ctx, std::string(host), make_ssl_context_for_http2(tcp_options.additional_ssl_certificates),
        tcp_options);
    co_return asio_connection{std::move(con)};
  }
  void run() {
    if (io_ctx->stopped())
      io_ctx->restart();
    io_ctx->run();
  }
  bool run_one(duration_t timeout) {
    if (io_ctx->stopped())
      io_ctx->restart();
    return (bool)io_ctx->run_one_for(timeout);
  }
  void stop() {
    io_ctx->stop();
  }
  dd::task<void> sleep(duration_t d, io_error_code& ec) {
    assert(io_ctx);
    asio::steady_timer t(*io_ctx);
    co_await net.sleep(t, d, ec);
  }
};

}  // namespace tgbm
