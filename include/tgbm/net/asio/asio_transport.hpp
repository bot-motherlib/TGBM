#pragma once

#include <boost/asio/io_context.hpp>

#undef NO_ERROR
#undef Yield
#undef min
#undef max

#include <kelcoro/thread_pool.hpp>

#include <tgbm/net/asio/tcp_connection.hpp>
#include <tgbm/net/transport_factory.hpp>

namespace tgbm {

struct asio_transport {
 private:
  asio::io_context io_ctx;
  tcp_connection_options tcp_options;

 public:
  explicit asio_transport(tcp_connection_options opts = {});

  asio_transport(asio_transport&&) = delete;
  void operator=(asio_transport&&) = delete;

  dd::task<any_connection> create_connection(std::string_view host);
  void run();
  bool run_one(duration_t timeout);
  void stop();
  dd::task<void> sleep(duration_t d, io_error_code& ec);
};

struct asio_acceptor {
  boost::asio::ip::tcp::acceptor acceptor;
  std::atomic_size_t& opened_sessions;

  asio_acceptor(boost::asio::io_context& io_ctx, endpoint_t ep, std::atomic_size_t& os)
      : acceptor(io_ctx, std::move(ep)), opened_sessions(os) {
    asio::socket_base::reuse_address(true);
  }
  asio_acceptor(asio_acceptor&&) = delete;
  void operator=(asio_acceptor&&) = delete;

  dd::task<any_connection> accept(io_error_code& ec);
};

struct asio_server_transport {
 private:
  tcp_connection_options tcp_options;
  boost::asio::io_context io_ctx;

  using work_guard_t = decltype(asio::make_work_guard(io_ctx));

  std::shared_ptr<work_guard_t> work_guard = nullptr;
  dd::thread_pool tp;
  std::atomic_size_t opened_sessions = 0;

 public:
  explicit asio_server_transport(tcp_connection_options opts = {},
                                 size_t listen_thread_count = std::thread::hardware_concurrency());

  asio_server_transport(asio_server_transport&&) = delete;
  void operator=(asio_server_transport&&) = delete;

  // any_server_transport_factory interface

  void run();
  bool run_one(duration_t timeout);
  void stop();
  any_acceptor make_acceptor(endpoint_t);
  bool start();
};

}  // namespace tgbm
