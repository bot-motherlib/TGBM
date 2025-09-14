#pragma once

#include <boost/system/error_code.hpp>
#undef Yield
#undef NO_ERROR
#include <exception>

#include <fmt/core.h>

#include <tgbm/utils/macro.hpp>

namespace tgbm {

using io_error_code = boost::system::error_code;

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

}  // namespace tgbm
