#pragma once

#include <string>
#include <string_view>

#include <kelcoro/task.hpp>

#include <anyany/anyany.hpp>

#include "tgbm/net/HttpParser.h"
#include "tgbm/tools/fn_ref.hpp"
#include "tgbm/tools/memory.hpp"
#include "tgbm/tools/deadline.hpp"

namespace tgbm {

using on_header_fn_ptr = fn_ptr<void(std::string_view name, std::string_view value)>;

using on_data_part_fn_ptr = fn_ptr<void(std::span<const byte_t> bytes, bool last_part)>;

struct reqerr_e {
  enum values : int {
    done = 0,             // setted when !on_header && !on_data_part, so no status parsed, but success
    cancelled = -1,       // e.g. handle.destroy() in http2
    user_exception = -2,  // some user generated exception, task->promise().exception setted
    timeout = -3,         //
    network_err = -4,     //
    protocol_err = -5,    // http protocol error
    server_cancelled_request = -6,  // for example rst_stream received
    unknown_err = -7,               // something like bad alloc maybe
  };
};

std::string_view e2str(reqerr_e::values e) noexcept;

enum struct http_method_e {
  GET,
  POST,
  PUT,
  DELETE,
  PATCH,
  OPTIONS,
  HEAD,
  CONNECT,
  TRACE,
};

std::string_view e2str(http_method_e e) noexcept;

struct http_header_t {
  std::string name;
  std::string value;
};

// client knows authority and scheme and sets it
struct http_request {
  // usually something like /bot<token>/<method>
  // must be setted to not empty string
  std::string path;
  http_method_e method = http_method_e::GET;
  http_body body;
  std::vector<http_header_t> headers;  // additional headers, all must be lowercase for http2
};

struct http_response {
  int status = 0;
  std::vector<http_header_t> headers;
  bytes_t body;
};

struct http_client {
 protected:
  std::string host;

 public:
  std::string_view get_host() const noexcept {
    return host;
  }

  http_client(std::string_view host);

  virtual ~http_client() = default;

  // rethrows exceptions from 'on_header' and 'on_data_part' to caller
  // if 'on_header' is nullptr, all headers ignored (status parsed if 'on_data_part' != nullptr)
  // if 'on_data_part' is nullptr, then server answer ignored
  // if both nullptr, then request only sended and then returns immediately
  // returns < 0 if error (reqerr_e),
  // 0 if request done, but both handlers nullptr and status not parsed
  // > 0 if 3-digit server response code
  virtual dd::task<int> send_request(on_header_fn_ptr on_header, on_data_part_fn_ptr on_data_part,
                                     http_request request, duration_t timeout) = 0;

  // throws on errors
  dd::task<http_response> send_request(http_request request, duration_t timeout);

  // run until all work done
  virtual void run() = 0;
  // run until some work done, returns false if no one executed
  virtual bool run_one(duration_t timeout) = 0;
  virtual void stop() = 0;
};

}  // namespace tgbm
