#include "tgbm/net/http_client.hpp"
#include "tgbm/net/errors.hpp"

namespace tgbm {

http_client::http_client(std::string_view host) : host(host) {
  if (host.empty() || host.starts_with("https://") || host.starts_with("http://"))
    throw std::invalid_argument("host should not be full url, correct example: \"api.telegram.org\"");
}

dd::task<http_response> http_client::send_request(http_request request, duration_t timeout) {
  http_response rsp;
  auto on_header = [&](std::string_view name, std::string_view value) {
    rsp.headers.emplace_back(std::string(name), std::string(value));
  };
  auto on_data_part = [&](std::span<const byte_t> bytes, bool last_part) {
    rsp.body.insert(rsp.body.end(), bytes.begin(), bytes.end());
  };
  rsp.status = co_await send_request(&on_header, &on_data_part, std::move(request), timeout);
  if (rsp.status < 0) {
    switch (reqerr_e::values(rsp.status)) {
      case reqerr_e::done:
      case reqerr_e::user_exception:
        KELCORO_UNREACHABLE;
      case reqerr_e::timeout:
        throw timeout_exception{};
      case reqerr_e::network_err:
        throw network_exception{""};
      case reqerr_e::protocol_err:
        throw protocol_error{};
      case reqerr_e::cancelled:
        throw std::runtime_error("HTTP client: request was canceled");
      case reqerr_e::server_cancelled_request:
        throw std::runtime_error("HTTP client: request was canceled by server");
      case reqerr_e::unknown_err:
        throw std::runtime_error("HTTP client unknown error happens");
    }
  }
  co_return std::move(rsp);
}

}  // namespace tgbm
