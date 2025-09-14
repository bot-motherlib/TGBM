#pragma once

#include <http2/http2_client.hpp>
#include <http2/http2_client_options.hpp>

#include "http2/asio/awaiters.hpp"
#include "tgbm/net/http_base.hpp"
#include "tgbm/net/http_client.hpp"

namespace tgbm {

struct http2_client : http_client {
 protected:
  http2::http2_client impl;
  std::string host;

 public:
  explicit http2_client(std::string_view host = "api.telegram.org", http2_client_options opts = {},
                        asio::ip::port_type port = 443)
      : impl(http2::endpoint(std::string(host), port), opts,
             [](asio::io_context& ctx) { return http2::default_tls_transport_factory(ctx); }),
        host(host) {
  }

  http2_client(http2_client&&) = delete;
  void operator=(http2_client&&) = delete;

  std::string_view get_host() const noexcept override {
    return host;
  }

  const http2_client_options& get_options() const noexcept {
    return impl.get_options();
  }

  ~http2_client() = default;

  dd::task<http_response> send_request(http_request req, deadline_t deadline) {
    return impl.send_request(std::move(req), deadline);
  }

  dd::task<http_response> send_request(http_request request, duration_t timeout) {
    return send_request(std::move(request), deadline_after(timeout));
  }

  dd::task<int> send_request(on_header_fn_ptr on_header, on_data_part_fn_ptr on_data_part, http_request req,
                             deadline_t deadline) override {
    req.authority = host;
    return impl.send_request(on_header, on_data_part, std::move(req), deadline);
  }

  dd::task<void> sleep(duration_t d, io_error_code& ec) override {
    asio::steady_timer timer(get_ioctx());
    co_await http2::net.sleep(timer, d, ec);
  }

  void run() override {
    if (impl.ioctx().stopped())
      impl.ioctx().restart();
    impl.ioctx().run();
  }

  bool run_one(duration_t timeout) override {
    if (impl.ioctx().stopped())
      impl.ioctx().restart();
    return impl.ioctx().run_one_for(timeout) != 0;
  }

  void stop() override {
    impl.cancel_all();
  }

  asio::io_context& get_ioctx() noexcept {
    return impl.ioctx();
  }
};

}  // namespace tgbm
