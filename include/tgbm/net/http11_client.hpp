#pragma once

#include <string_view>

#include "tgbm/net/http_client.hpp"
#include "tgbm/net/connectiion_pool.hpp"
#include "tgbm/logger.h"

#include "tgbm/net/asio_ssl_connection.hpp"

namespace tgbm {

namespace asio = boost::asio;

struct http11_client : http_client {
 private:
  // invariant: .run() invoked on 0 or 1 threads
  asio::io_context io_ctx;
  pool_t<std::shared_ptr<asio_ssl_connection>> connections;
  KELCORO_NO_UNIQUE_ADDRESS dd::this_thread_executor_t exe;
  size_t requests_in_progress = 0;
  bool stop_requested = false;

 public:
  explicit http11_client(size_t connections_max_count = 64, std::string_view host = "api.telegram.org");

  dd::task<int> send_request(on_header_fn_ptr on_header, on_data_part_fn_ptr on_data_part, http_request,
                             duration_t timeout) override;

  void run() override;

  bool run_one(duration_t timeout) override;

  void stop() override;
};

}  // namespace tgbm
