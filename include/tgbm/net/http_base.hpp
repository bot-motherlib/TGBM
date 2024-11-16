#pragma once

#include <string_view>
#include <vector>

#include <tgbm/net/http_body.hpp>

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

// does nothing for good status, rethrows error (timeout, http_exception etc) for bad status
void handle_telegram_http_status(int status);

std::string_view e2str(reqerr_e::values e) noexcept;

enum struct http_method_e : uint8_t {
  GET,
  POST,
  PUT,
  DELETE_,
  PATCH,
  OPTIONS,
  HEAD,
  CONNECT,
  TRACE,
  UNKNOWN,  // may be extension or smth
};

std::string_view e2str(http_method_e e) noexcept;
void enum_from_string(std::string_view, http_method_e&) noexcept;

enum struct scheme_e : uint8_t {
  HTTP,
  HTTPS,
  UNKNOWN,
};

std::string_view e2str(scheme_e e) noexcept;
void enum_from_string(std::string_view, scheme_e&) noexcept;

struct http_header_t {
  std::string name;
  std::string value;
};

// client knows authority and scheme and sets it
struct http_request {
  // Host for HTTP1/1, :authority for HTTP2
  std::string authority = {};
  // usually something like /bot<token>/<method>
  // must be setted to not empty string
  std::string path;
  http_method_e method = http_method_e::UNKNOWN;
  // 'scheme' is for server, clients will ignore it and use their scheme instead
  scheme_e scheme = scheme_e::UNKNOWN;
  http_body body = {};
  // additional headers, all must be lowercase for HTTP2
  std::vector<http_header_t> headers;
};

struct http_response {
  int status = 0;
  std::vector<http_header_t> headers;
  bytes_t body;
};

}  // namespace tgbm
