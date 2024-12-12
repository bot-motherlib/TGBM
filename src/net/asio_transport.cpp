#include <tgbm/net/asio_transport.hpp>
#include <tgbm/net/asio_awaiters.hpp>
#include <tgbm/utils/zero_movabe.hpp>

namespace tgbm {

asio_transport::asio_transport(tcp_connection_options opts) : io_ctx(1), tcp_options(std::move(opts)) {
}

dd::task<any_connection> asio_transport::create_connection(std::string_view host) {
  co_return co_await tcp_connection::create(io_ctx, std::string(host), tcp_options);
}

void asio_transport::run() {
  if (io_ctx.stopped())
    io_ctx.restart();
  io_ctx.run();
}

bool asio_transport::run_one(duration_t timeout) {
  if (io_ctx.stopped())
    io_ctx.restart();
  return (bool)io_ctx.run_one_for(timeout);
}

void asio_transport::stop() {
  io_ctx.stop();
}

dd::task<void> asio_transport::sleep(duration_t d, io_error_code& ec) {
  asio::steady_timer t(io_ctx);
  co_await net.sleep(t, d, ec);
}

void asio_server_transport::run() {
  if (io_ctx.stopped())
    io_ctx.restart();
  io_ctx.run();
}

bool asio_server_transport::run_one(duration_t timeout) {
  if (io_ctx.stopped())
    io_ctx.restart();
  return (bool)io_ctx.run_one_for(timeout);
}

void asio_server_transport::stop() {
  if (!work_guard)
    return;
  (*work_guard).reset();
  work_guard = nullptr;
  io_ctx.stop();
}

any_acceptor asio_server_transport::make_acceptor(endpoint_t ep) {
  return make_any_acceptor<asio_acceptor>(io_ctx, std::move(ep), opened_sessions);
}

bool asio_server_transport::start() {
  if (work_guard)
    return false;
  if (io_ctx.stopped()) {
    // drop old sessions before starting new ones
    io_ctx.restart();
    while (opened_sessions.load(std::memory_order_acquire) != 0)
      io_ctx.run_one();
  }
  work_guard = std::shared_ptr<work_guard_t>(new auto(asio::make_work_guard(io_ctx)));
  for (dd::task_queue& q : tp.queues_range())
    dd::schedule_to(q, [&ctx = this->io_ctx, g = work_guard] { ctx.run(); });
  return true;
}

struct tcp_connection_with_ref : tcp_connection {
  zero_movable<std::atomic_size_t*> opened_sessions;

  tcp_connection_with_ref(tcp_connection c, std::atomic_size_t& os)
      : tcp_connection(std::move(c)), opened_sessions(&os) {
  }

  tcp_connection_with_ref(tcp_connection_with_ref&&) = default;
  tcp_connection_with_ref& operator=(tcp_connection_with_ref&&) = default;

  void shutdown() {
    if (!opened_sessions.value)
      return;
    tcp_connection::shutdown();
    opened_sessions.value->fetch_sub(1, std::memory_order_acq_rel);
    opened_sessions.value = nullptr;
  }
  ~tcp_connection_with_ref() {
    shutdown();
  }
};

dd::task<any_connection> asio_acceptor::accept(io_error_code& ec) {
  asio::ip::tcp::socket sock(asio::make_strand(acceptor.get_executor()));

  TGBM_LOG_INFO("[TCP] server start accepting on {}, port: {}",
                acceptor.local_endpoint().address().to_string(), acceptor.local_endpoint().port());
  acceptor.listen();
  co_await net.accept(acceptor, sock, ec);

  if (ec)
    co_return {};

  opened_sessions.fetch_add(1, std::memory_order_acq_rel);
  // TODO make any
  co_return tcp_connection_with_ref(tcp_connection(std::move(sock)), opened_sessions);
}

asio_server_transport::asio_server_transport(tcp_connection_options opts, size_t listen_thread_count)
    : tcp_options(std::move(opts)), io_ctx(), tp(listen_thread_count) {
}

}  // namespace tgbm
