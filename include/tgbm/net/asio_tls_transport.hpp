#pragma once

#include <atomic>
#include <filesystem>

#include <boost/asio/io_context.hpp>

#undef NO_ERROR
#undef Yield
#undef min
#undef max

#include <kelcoro/thread_pool.hpp>

#include <tgbm/net/tcp_connection.hpp>
#include <tgbm/net/transport_factory.hpp>
#include <tgbm/net/asio_tls_transport.hpp>

namespace tgbm {

struct asio_tls_transport {
 private:
  boost::asio::io_context io_ctx;
  tcp_connection_options tcp_options;

 public:
  explicit asio_tls_transport(tcp_connection_options opts = {});

  asio_tls_transport(asio_tls_transport&&) = delete;
  void operator=(asio_tls_transport&&) = delete;

  dd::task<any_connection> create_connection(std::string_view host);
  void run();
  bool run_one(duration_t timeout);
  void stop();
  dd::task<void> sleep(duration_t d, io_error_code& ec);
};

struct asio_tls_acceptor {
  ssl_context_ptr sslctx;
  boost::asio::ip::tcp::acceptor acceptor;
  std::atomic_size_t& opened_sessions;

  asio_tls_acceptor(boost::asio::io_context& io_ctx, ssl_context_ptr ssl, endpoint_t ep,
                    std::atomic_size_t& os) noexcept
      : sslctx(std::move(ssl)), acceptor(io_ctx, std::move(ep)), opened_sessions(os) {
    acceptor.set_option(asio::socket_base::reuse_address(true));
  }
  asio_tls_acceptor(asio_tls_acceptor&&) = delete;
  void operator=(asio_tls_acceptor&&) = delete;

  dd::task<any_connection> accept(io_error_code& ec);
};

struct asio_server_tls_transport {
 private:
  ssl_context_ptr sslctx;
  tcp_connection_options tcp_options;
  boost::asio::io_context io_ctx;

  using work_guard_t = decltype(asio::make_work_guard(io_ctx));

  std::shared_ptr<work_guard_t> work_guard = nullptr;
  dd::thread_pool tp;
  std::atomic_size_t opened_sessions = 0;

  friend struct asio_server_transport;

 public:
  explicit asio_server_tls_transport(std::filesystem::path ssl_cert_path,
                                     std::filesystem::path private_key_path, tcp_connection_options opts = {},
                                     size_t listen_thread_count = std::thread::hardware_concurrency());

  asio_server_tls_transport(asio_server_tls_transport&&) = delete;
  void operator=(asio_server_tls_transport&&) = delete;

  // any_server_transport_factory interface

  void run();
  bool run_one(duration_t timeout);
  void stop();
  any_acceptor make_acceptor(endpoint_t);
  bool start();
};

}  // namespace tgbm
