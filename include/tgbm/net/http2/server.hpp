#pragma once

#include <boost/asio/ip/address.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/smart_ptr/intrusive_ptr.hpp>
#include <boost/asio/io_context.hpp>

#undef NO_ERROR
#undef Yield
#undef min
#undef max

#include <kelcoro/job.hpp>
#include <kelcoro/task.hpp>
#include <kelcoro/thread_pool.hpp>
#include <anyany/anyany.hpp>

#include <tgbm/net/http_base.hpp>
#include <tgbm/net/tcp_connection.hpp>
#include <tgbm/utils/deadline.hpp>
#include <tgbm/utils/memory.hpp>
#include <tgbm/net/transport_factory.hpp>

namespace tgbm {

namespace asio = boost::asio;

struct http_server {
  virtual dd::task<http_response> handle_request(http_request) = 0;
  virtual ~http_server() = default;
};

struct http2_server_options {
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

struct http2_server : http_server {
 private:
  http2_server_options options;
  any_server_transport_factory transport;
  std::atomic<size_t> refcount = 0;
  std::atomic_bool _stop_requested = false;

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
  explicit http2_server(any_server_transport_factory, http2_server_options = {});

  // TODO ctor with ssl certificates / private key pathes

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
  // must not be called concurrently
  void run();

  void stop();
};

}  // namespace tgbm
