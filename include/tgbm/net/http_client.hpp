#pragma once

#include <string>
#include <string_view>

#include <kelcoro/task.hpp>

#include <tgbm/net/HttpParser.h>

namespace tgbm {

using duration_t = std::chrono::steady_clock::duration;

struct http_client {
 protected:
  std::string host;

 public:
  std::string_view get_host() const noexcept {
    return host;
  }

  http_client(std::string_view host);

  virtual ~http_client() = default;

  virtual dd::task<http_response> send_request(http_request request, duration_t timeout) = 0;

  // run until all work done
  virtual void run() = 0;
  // run until some work done, returns false if no one executed
  virtual bool run_one(duration_t timeout) = 0;
  virtual void stop() = 0;
};

}  // namespace tgbm
