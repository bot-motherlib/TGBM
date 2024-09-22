#pragma once

#include "tgbm/net/http_client.hpp"
#include "tgbm/net/http2/errors.hpp"

#include <kelcoro/job.hpp>

#include <boost/intrusive/list_hook.hpp>
#include <boost/intrusive/list.hpp>
// TODO fix correct order of includes, windows macros(fuck them)
#include "tgbm/net/asio_ssl_connection.hpp"
#include <boost/asio/io_context.hpp>

#include "tgbm/tools/boost_intrusive.hpp"

namespace tgbm {

struct http2_client;
struct http2_connection;

namespace noexport {

struct waiter_of_connection : bi::list_base_hook<link_option> {
  std::coroutine_handle<> task;
  http2_client* client = nullptr;
  std::shared_ptr<http2_connection> result;
  TGBM_PIN;

  explicit waiter_of_connection(http2_client* c) noexcept : client(c) {
  }

  ~waiter_of_connection();

  bool await_ready() noexcept;
  std::coroutine_handle<> await_suspend(std::coroutine_handle<>) noexcept;
  [[nodiscard]] std::shared_ptr<http2_connection> await_resume() const;
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

struct http2_client : http_client {
 private:
  friend noexport::waiter_of_connection;

  // invariant: .run() invoked on 0 or 1 threads
  asio::io_context io_ctx;
  std::shared_ptr<http2_connection> connection;
  // while connection is not ready all new streams wait for it
  bi::list<noexport::waiter_of_connection, bi::cache_begin<true>, bi::cache_last<true>> connection_waiters;
  size_t requests_in_progress = 0;
  bool is_connecting = false;  // if connection started to establish, but not established yet
  bool stop_requested = false;
  bool someone_waits_completely_done = false;

  // fills requests from raw http2 frames
  dd::job start_reader_for(std::shared_ptr<http2_connection>);
  // writes requests
  dd::job start_writer_for(std::shared_ptr<http2_connection>);
  // postcondiiton: returns not null, !returned->dropped && returned->stream_id <= max_stream_id
  // && !client.stop_requestedg
  [[nodiscard]] noexport::waiter_of_connection borrow_connection() noexcept {
    return noexport::waiter_of_connection(this);
  }

  void notify_connection_waiters(std::shared_ptr<http2_connection> result) noexcept;

  // postcondition: !connection
  void drop_connection() noexcept;

  [[nodiscard]] bool already_connecting() const noexcept {
    return is_connecting;
  }
  [[nodiscard]] noexport::connection_lock_t lock_connections() noexcept {
    return noexport::connection_lock_t(is_connecting);
  }

  // invariant: only one may be called at one time, 'is_connecting' flag setted only if now in progress
  [[nodiscard("this handle must be resumed")]] dd::job start_connecting();

 public:
  explicit http2_client(std::string_view host = "api.telegram.org");

  http2_client(http2_client&&) = delete;
  void operator=(http2_client&&) = delete;

  ~http2_client() override;

  // Note: .destroy on handle works as canceling
  // Note: timeout is for reading side (from TG),
  // now sending big files will not be canceled by timeout (but receiving answer - will be)
  dd::task<http_response> send_request(http_request, duration_t timeout) override;

  void run() override;

  bool run_one(duration_t timeout) override;

  void stop() override;
};

}  // namespace tgbm
