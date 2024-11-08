#pragma once

#include "tgbm/net/http_client.hpp"
#include "tgbm/net/asio_awaiters.hpp"

namespace tgbm {

template <std::derived_from<http_client> Client>
struct http_client_with_retries : Client {
  uint32_t retry_count = 1;
  duration_t wait_between_retries = std::chrono::milliseconds(250);

  using Client::Client;
  using Client::operator=;

 private:
  dd::task<bool> sleep(duration_t duration) {
    if constexpr (requires { this->io_ctx; }) {
      asio::steady_timer timer(this->io_ctx);
      io_error_code ec;
      co_await net.sleep(timer, duration, ec);
      if (ec && ec != asio::error::operation_aborted)
        co_return false;
    } else {
      std::this_thread::sleep_for(duration);
    }
    co_return true;
  }

 public:
  dd::task<int> send_request(on_header_fn_ptr on_header, on_data_part_fn_ptr on_data_part,
                             http_request request, deadline_t deadline) override {
    // avoid invoking on_header / on_data_part incorrectly after retry.
    // so, if one of callbacks called once and failed, do not retry again
    // effectively retries after network / protocol errors

    bool called = false;
    auto on_header_wrapped = [on_header, &called](std::string_view name, std::string_view value) {
      called = true;
      (*on_header)(name, value);
    };
    auto on_data_part_wrapped = [on_data_part, &called](std::span<const byte_t> bytes, bool is_last_chunk) {
      called = true;
      (*on_data_part)(bytes, is_last_chunk);
    };
    if (on_header)
      on_header = &on_header_wrapped;
    if (on_data_part)
      on_data_part = &on_data_part_wrapped;

    // do send request

    uint32_t retry_c = retry_count;
    int status;
  retry:
    status = co_await Client::send_request(on_header, on_data_part, request, deadline);
    if (!called && status < 0 && status != reqerr_e::timeout && retry_c > 0) {
      --retry_c;
      if (wait_between_retries.count() > 0)
        (void)co_await sleep(wait_between_retries);
      goto retry;
    }
    co_return status;
  }
};

}  // namespace tgbm
