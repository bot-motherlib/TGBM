#include <tgbm/net/asio/asio_tls_transport.hpp>
#include <tgbm/net/asio/asio_awaiters.hpp>
#include <tgbm/utils/scope_exit.hpp>

namespace tgbm {

asio_tls_transport::asio_tls_transport(tcp_connection_options opts)
    : io_ctx(1), tcp_options(std::move(opts)) {
  if (tcp_options.disable_ssl_certificate_verify)
    TGBM_LOG_WARN("SSL veriication for http2 client disabled");
}

dd::task<any_connection> asio_tls_transport::create_connection(std::string_view host, deadline_t deadline) {
  // TODO reuse ssl context ?
  tcp_tls_connection con = co_await tcp_tls_connection::create(
      io_ctx, std::string(host), make_ssl_context_for_http2(tcp_options.additional_ssl_certificates),
      deadline, tcp_options);
  co_return tcp_tls_connection{std::move(con)};
}

void asio_tls_transport::run() {
  if (io_ctx.stopped())
    io_ctx.restart();
  io_ctx.run();
}

bool asio_tls_transport::run_one(duration_t timeout) {
  if (io_ctx.stopped())
    io_ctx.restart();
  return (bool)io_ctx.run_one_for(timeout);
}

void asio_tls_transport::stop() {
  auto tmrs = std::move(timers);
  for (asio::steady_timer* tmr : tmrs) {
    assert(tmr);
    tmr->cancel();
  }
  io_ctx.stop();
}

dd::task<void> asio_tls_transport::sleep(duration_t d, io_error_code& ec) {
  asio::steady_timer t(io_ctx);
  timers.insert(&t);
  on_scope_exit {
    timers.erase(&t);
  };
  co_await net.sleep(t, d, ec);
}

asio_server_tls_transport::asio_server_tls_transport(std::filesystem::path ssl_cert_path,
                                                     std::filesystem::path private_key_path,
                                                     tcp_connection_options opts, size_t listen_thread_count)
    : sslctx(make_ssl_context_for_server(std::move(ssl_cert_path), std::move(private_key_path))),
      tcp_options(std::move(opts)),
      io_ctx(),
      tp(listen_thread_count) {
  if (!sslctx) {
    throw std::runtime_error(
        "[HTTP2] server cannot initialize sslctx! Set ssl cert path and private key path");
  }
}

void asio_server_tls_transport::run() {
  if (io_ctx.stopped())
    io_ctx.restart();
  io_ctx.run();
}

bool asio_server_tls_transport::run_one(duration_t timeout) {
  if (io_ctx.stopped())
    io_ctx.restart();
  return (bool)io_ctx.run_one_for(timeout);
}

void asio_server_tls_transport::stop() {
  if (!work_guard)
    return;
  (*work_guard).reset();
  work_guard = nullptr;
  io_ctx.stop();
}

any_acceptor asio_server_tls_transport::make_acceptor(endpoint_t ep) {
  return $inplace(asio_tls_acceptor(io_ctx, sslctx, std::move(ep), opened_sessions));
}

bool asio_server_tls_transport::start() {
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

struct tcp_tls_connection_with_ref : tcp_tls_connection {
  std::atomic_size_t* opened_sessions;

  tcp_tls_connection_with_ref(tcp_tls_connection c, std::atomic_size_t& os)
      : tcp_tls_connection(std::move(c)), opened_sessions(&os) {
  }

  tcp_tls_connection_with_ref(tcp_tls_connection_with_ref&&) = delete;
  tcp_tls_connection_with_ref& operator=(tcp_tls_connection_with_ref&&) = delete;

  void shutdown() {
    if (!opened_sessions)
      return;
    tcp_tls_connection::shutdown();
    opened_sessions->fetch_sub(1, std::memory_order_acq_rel);
    opened_sessions = nullptr;
  }
  ~tcp_tls_connection_with_ref() {
    shutdown();
  }
};

dd::task<any_connection> asio_tls_acceptor::accept(io_error_code& ec) {
  assert(sslctx);
  asio::ssl::stream<asio::ip::tcp::socket> sock(
      asio::ip::tcp::socket(asio::make_strand(acceptor.get_executor())), sslctx->ctx);
  co_await net.accept(acceptor, sock.lowest_layer(), ec);

  if (ec)
    co_return {};
  co_await net.handshake(sock, asio::ssl::stream_base::server, ec);
  if (ec)
    co_return {};
  opened_sessions.fetch_add(1, std::memory_order_acq_rel);

  co_return $inplace(
      tcp_tls_connection_with_ref(tcp_tls_connection(std::move(sock), sslctx), opened_sessions));
}

}  // namespace tgbm
