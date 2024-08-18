#pragma once

#include <string>

#include <fmt/format.h>

#include <tgbm/net/HttpParser.h>

#include <kelcoro/task.hpp>

namespace tgbm {

struct network_exception : std::exception {
  std::string data;

  template <typename... Args>
  explicit network_exception(fmt::format_string<Args...> fmt_str, Args&&... args)
      : data(fmt::format(fmt_str, std::forward<Args>(args)...)) {
  }
  explicit network_exception(std::string s) noexcept : data(std::move(s)) {
  }
  const char* what() const noexcept KELCORO_LIFETIMEBOUND override {
    return data.c_str();
  }
};

struct timeout_exception : network_exception {
  using network_exception::network_exception;
};
struct http_exception : network_exception {
  using network_exception::network_exception;
};

using duration_t = std::chrono::steady_clock::duration;

/**
 * @brief This class makes http requests.
 *
 * @ingroup net
 */
class HttpClient {
 public:
  virtual ~HttpClient() = default;

  virtual dd::task<http_response> send_request(http_request request, duration_t timeout) = 0;
  // run until all work done
  virtual void run() = 0;
  // run until some work done, returns false if no one executed
  virtual bool run_one(duration_t timeout) = 0;
  virtual void stop() = 0;
};

}  // namespace tgbm
