#pragma once

#include <string>
#include <cstdint>
#include <fmt/format.h>
#include <tgbm/net/HttpParser.h>

#include <kelcoro/task.hpp>

namespace tgbm {

struct network_exception : std::exception {
  std::string data;

  template <typename... Args>
  explicit network_exception(fmt::format_string<Args...> fmt_str, Args &&...args)
      : data(fmt::format(fmt_str, std::forward<Args>(args)...)) {
  }
  explicit network_exception(std::string s) noexcept : data(std::move(s)) {
  }
  const char *what() const noexcept KELCORO_LIFETIMEBOUND override {
    return data.c_str();
  }
};

struct http_exception : network_exception {
  using network_exception::network_exception;
};

/**
 * @brief This class makes http requests.
 *
 * @ingroup net
 */
class HttpClient {
 public:
  virtual ~HttpClient() = default;

  // TODO add prepare function?

  /**
   * @brief Sends a request to the url.
   *
   * If there's no args specified, a GET request will be sent, otherwise a POST request will be sent.
   * If at least 1 arg is marked as file, the content type of a request will be multipart/form-data, otherwise
   * it will be application/x-www-form-urlencoded.
   */
  virtual dd::task<http_response> makeRequest(http_request request) = 0;
  // TODO передавать таймаут в makeRequest, количество ретраев вероятно тоже. И std::chrono::seconds вместо
  // int
  std::int32_t _timeout = 25;

  // TODO remove rettries, timeout etc from here

  virtual int getRequestMaxRetries() const {
    return requestMaxRetries;
  }

  virtual int getRequestBackoff() const {
    return requestBackoff;
  }

 private:
  int requestMaxRetries = 3;
  int requestBackoff = 1;
};

}  // namespace tgbm
