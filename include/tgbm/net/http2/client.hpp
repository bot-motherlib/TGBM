#pragma once

#include <hidi/h2client.hpp>
#include <hidi/h2client_options.hpp>
#include <hidi/asio/awaiters.hpp>
#include <hidi/asio/io_impl.hpp>

#include "tgbm/net/http_base.hpp"
#include "tgbm/net/http_client.hpp"
#include "tgbm/net/tcp_starters.hpp"

namespace tgbm {

namespace asio = boost::asio;

struct http2_client_init {
  // each request :authority
  std::string host = "api.telegram.org";
  // destination to connect (usually equal to `host`)
  std::string dst = "api.telegram.org";
  uint16_t dstport = 443;
  http2_client_options options = {};
  hidi::tcp_connection_options tcp_options = {};
  starter_t starter = {};
};

struct http2_client : http_client {
 protected:
  hidi::h2client impl;
  std::string host;

 public:
  explicit http2_client(std::string_view host = "api.telegram.org", http2_client_options opts = {},
                        asio::ip::port_type port = 443);

  explicit http2_client(http2_client_init init);

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
    hidi::any_timer timer = get_ioctx().create_timer();
    co_await hidi::net.sleep(timer, d, ec);
  }

  void run() override {
    if (impl.ioctx().stopped())
      impl.ioctx().restart();
    impl.ioctx().run();
  }

  bool poll_one() override {
    if (impl.ioctx().stopped())
      impl.ioctx().restart();
    return impl.ioctx().poll_one();
  }

  void stop() override {
    impl.cancel_all();
  }

  hidi::any_io_context& get_ioctx() noexcept {
    return impl.ioctx();
  }
};

}  // namespace tgbm
