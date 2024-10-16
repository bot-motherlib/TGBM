#pragma once

#include <string_view>
#include <vector>

#include "tgbm/net/HttpParser.h"

namespace tgbm {

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
  DELETE_,
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

}  // namespace tgbm