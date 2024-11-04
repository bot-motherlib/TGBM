#pragma once

#include <boost/system/error_code.hpp>

#include <exception>

#include <fmt/core.h>

#include "tgbm/tools/macro.hpp"

namespace tgbm {

using io_error_code = boost::system::error_code;

struct network_exception : std::exception {
  std::string data;

  template <typename... Args>
  explicit network_exception(fmt::format_string<Args...> fmt_str, Args&&... args)
      : data(fmt::format(fmt_str, std::forward<Args>(args)...)) {
  }
  explicit network_exception(const io_error_code& ec) : data(fmt::format("{}", ec.what())) {
  }
  explicit network_exception(std::string s) noexcept : data(std::move(s)) {
  }
  const char* what() const noexcept KELCORO_LIFETIMEBOUND override {
    return data.c_str();
  }
};

struct timeout_exception : std::exception {
  const char* what() const noexcept override {
    return "timeout";
  }
};

struct http_exception : std::exception {
  int status = 0;

  explicit http_exception(int status) noexcept : status(status) {
  }
  const char* what() const noexcept override {
    return "http_exception";
  }
};

struct bad_request : http_exception {
  std::string description;

  explicit bad_request(std::string s) noexcept : http_exception(404), description(std::move(s)) {
  }

  const char* what() const noexcept override {
    return description.c_str();
  }
};

#define TGBM_TG_EXCEPTION(ERRC, FMT_STR, ...) \
  ::tgbm::tg_exception(FMT_STR " errc: {}" __VA_OPT__(, ) __VA_ARGS__, e2str(ERRC))

// thrown when Telegram refuses API request
struct tg_exception : network_exception {
  using network_exception::network_exception;
};

struct connection_shutted_down : std::exception {
  const char* what() const noexcept override {
    return "connection was shutted down";
  }
};

struct client_stopped : std::exception {
  const char* what() const noexcept {
    return "http client was stopped";
  }
};

struct protocol_error : std::exception {
  const char* what() const noexcept override {
    return "protocol error";
  }
};

struct unimplemented : std::exception {
  const char* msg = nullptr;
  unimplemented(const char* msg) noexcept : msg(msg) {
  }
  const char* what() const noexcept override {
    return msg;
  }
};

}  // namespace tgbm
