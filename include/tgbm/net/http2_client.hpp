#pragma once

#include "tgbm/net/http_client.hpp"

#include <kelcoro/job.hpp>

#include <boost/intrusive/list_hook.hpp>
#include <boost/intrusive/list.hpp>
// TODO fix correct order of includes, windows macros(fuck them)
#include "tgbm/net/tcp_connection.hpp"
#include <boost/asio/io_context.hpp>
#include <boost/smart_ptr/intrusive_ptr.hpp>

#include "tgbm/tools/boost_intrusive.hpp"

namespace tgbm {

struct http2_client;
struct http2_connection;

void intrusive_ptr_add_ref(http2_connection*);
void intrusive_ptr_release(http2_connection*) noexcept;

using http2_connection_ptr = boost::intrusive_ptr<http2_connection>;

namespace noexport {

struct waiter_of_connection : bi::list_base_hook<link_option> {
  std::coroutine_handle<> task;
  http2_client* client = nullptr;
  http2_connection_ptr result = nullptr;
  TGBM_PIN;

  explicit waiter_of_connection(http2_client* c) noexcept : client(c) {
  }

  ~waiter_of_connection();

  bool await_ready() noexcept;
  std::coroutine_handle<> await_suspend(std::coroutine_handle<>) noexcept;
  [[nodiscard]] http2_connection_ptr await_resume() const;
};

struct connection_lock_t {
 private:
  bool* is_connecting;

 public:
  explicit connection_lock_t(bool& b) : is_connecting(&b) {
    assert(!b);
    *is_connecting = true;
  }
  void release() {
    *is_connecting = false;
  }
  ~connection_lock_t() {
    release();
  }
};

}  // namespace noexport
}  // namespace tgbm

namespace tgbm {

struct http2_client_options {
  // may be used to handle requests while sending big frames, such as files data
  // must not be 0
  uint32_t max_send_frame_size = 8 * 1024;  // 8 KB
  uint32_t max_receive_frame_size = uint32_t(-1);
  uint32_t hpack_dyntab_size = 4096;
  // sends ping when there are no requests(for keeping alive). disabled by default
  duration_t ping_interval = duration_t::max();
  // duration_t::max() disables timeouts
  duration_t timeout_check_interval = std::chrono::seconds(1);
};

struct http2_client : http_client {
 protected:
  friend noexport::waiter_of_connection;

  TGBM_DEBUG_FIELD(threadid, std::thread::id);
  http2_client_options options;
  // invariant: .run() invoked on 0 or 1 threads
  asio::io_context io_ctx;
  http2_connection_ptr connection;
  tcp_connection_options tcp_options;
  // while connection is not ready all new streams wait for it
  bi::list<noexport::waiter_of_connection, bi::cache_last<true>> connection_waiters;
  size_t requests_in_progress = 0;
  bool is_connecting = false;  // if connection started to establish, but not established yet
  bool stop_requested = false;

  // fills requests from raw http2 frames
  dd::job start_reader_for(http2_connection_ptr);
  // writes requests
  dd::job start_writer_for(http2_connection_ptr);
  // postcondiiton: returns not null, !returned->dropped && returned->stream_id <= max_stream_id
  // && !client.stop_requestedg
  [[nodiscard]] noexport::waiter_of_connection borrow_connection() noexcept {
    return noexport::waiter_of_connection(this);
  }

  void notify_connection_waiters(http2_connection_ptr result) noexcept;

  // postcondition: !connection
  void drop_connection(reqerr_e::values reason) noexcept;

  [[nodiscard]] bool already_connecting() const noexcept {
    return is_connecting;
  }
  [[nodiscard]] noexport::connection_lock_t lock_connections() noexcept {
    return noexport::connection_lock_t(is_connecting);
  }

  // invariant: only one may be called at one time, 'is_connecting' flag setted only if now in progress
  [[nodiscard("this handle must be resumed")]] dd::job start_connecting();

 public:
  explicit http2_client(std::string_view host = "api.telegram.org", http2_client_options = {},
                        tcp_connection_options = {});

  http2_client(http2_client&&) = delete;
  void operator=(http2_client&&) = delete;

  const http2_client_options& get_options() const noexcept {
    return options;
  }

  ~http2_client() override;

  using http_client::send_request;
  dd::task<int> send_request(on_header_fn_ptr on_header, on_data_part_fn_ptr on_data_part, http_request,
                             deadline_t deadline) override;

  void run() override;

  bool run_one(duration_t timeout) override;

  void stop() override;
};

}  // namespace tgbm
