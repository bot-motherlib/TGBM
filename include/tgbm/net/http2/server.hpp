#pragma once

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/address.hpp>

#include <boost/asio/ip/tcp.hpp>

#include <filesystem>

#include <kelcoro/job.hpp>
#include <kelcoro/task.hpp>
#include <kelcoro/thread_pool.hpp>
#include <anyany/anyany.hpp>

#include "boost/smart_ptr/intrusive_ptr.hpp"
#include "tgbm/net/http_base.hpp"
#include "tgbm/net/tcp_connection.hpp"
#include "tgbm/utils/deadline.hpp"
#include "tgbm/utils/memory.hpp"

namespace tgbm {

namespace asio = boost::asio;

struct http_server {
  virtual dd::task<http_response> handle_request(http_request) = 0;
  virtual ~http_server() = default;
};

struct http2_server_options {
  // required to set
  std::filesystem::path ssl_cert_path;
  std::filesystem::path private_key_path;
  uint32_t max_send_frame_size = 8 * 1024;  // 8 KB
  uint32_t max_receive_frame_size = uint32_t(-1);
  uint32_t hpack_dyntab_size = 4096;
  uint32_t initial_stream_window_size = -1;
  uint32_t max_concurrent_stream_per_connection = -1;
  // TODO use
  duration_t idle_timeout;  // when drop client if it not send anything
  // TODO ? somehow handle overloading, мб через таймауты на отправку?..
  // хотя самое логичное это как то детектить перегрузку и обрывать новые стримы если перегруз
  // (settings и поставить макс конкурент стрим 0)
};

struct http2_server;

using http2_server_ptr = boost::intrusive_ptr<http2_server>;

struct ssl_context;
using ssl_context_ptr = boost::intrusive_ptr<ssl_context>;

struct http2_server : http_server {
 private:
  asio::io_context io_ctx;
  http2_server_options options;
  tcp_connection_options tcp_options;
  ssl_context_ptr sslctx = nullptr;
  std::atomic<size_t> refcount = 0;
  std::atomic_bool _stop_requested = false;
  // accepts on all threads, but each connection works only on one worker
  // by value, because work endlessly on io_ctx
  dd::thread_pool tp;
  using work_guard_t = decltype(asio::make_work_guard(io_ctx));
  std::shared_ptr<work_guard_t> work_guard = nullptr;
  friend struct client_session;
  std::atomic_size_t opened_sessions = 0;

  /*
    acceptы on all threads, then creates session
    which works on one thread
  */
  dd::job start_accept(asio::ip::tcp::endpoint);

  friend void intrusive_ptr_add_ref(http2_server* server) noexcept {
    server->refcount.fetch_add(1, std::memory_order_acq_rel);
  }
  friend void intrusive_ptr_release(http2_server* server) noexcept {
    if (server->refcount.fetch_sub(1, std::memory_order_acq_rel) == 1)
      delete server;
  }

 protected:
  // protected, must not be created on stack
  ~http2_server() {
    stop();
  }

 public:
  explicit http2_server(http2_server_options, tcp_connection_options = {},
                        size_t listen_thread_count = std::thread::hardware_concurrency());

  http2_server(http2_server&&) = delete;
  void operator=(http2_server&&) = delete;

  const http2_server_options& get_options() const noexcept {
    return options;
  }
  // adds addr to listen addrs
  void listen(asio::ip::tcp::endpoint);
  void listen(asio::ip::address addr, asio::ip::port_type port = 443) {
    return listen({addr, port});
  }

  [[nodiscard]] bool stop_requested() const noexcept {
    return _stop_requested.load(std::memory_order_acquire);
  }
  // not blocking, starts listening on other threads
  // returns false if already started
  bool start();
  // start + blocking
  void run();
  void stop();
};

}  // namespace tgbm
