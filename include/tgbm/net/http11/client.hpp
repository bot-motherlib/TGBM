#pragma once

#include <string_view>

#include <tgbm/net/http_client.hpp>
#include <tgbm/net/http11/connection_pool.hpp>

#include <tgbm/net/asio/tcp_connection.hpp>

namespace tgbm {

namespace asio = boost::asio;

struct http11_client : http_client {
 protected:
  // invariant: .run() invoked on 0 or 1 threads
  asio::io_context io_ctx;
  tcp_connection_options tcp_options;
  pool_t<tcp_tls_connection> connections;
  size_t requests_in_progress = 0;
  bool stop_requested = false;

 public:
  explicit http11_client(size_t connections_max_count = 64, std::string_view host = "api.telegram.org",
                         tcp_connection_options = {});

  using http_client::send_request;
  // note: this client ignores timeouts, its propose to just work in any case
  dd::task<int> send_request(on_header_fn_ptr on_header, on_data_part_fn_ptr on_data_part, http_request,
                             deadline_t deadline) override;

  dd::task<void> sleep(duration_t, io_error_code&) override;

  void run() override;

  bool run_one(duration_t timeout) override;

  void stop() override;
};

}  // namespace tgbm
