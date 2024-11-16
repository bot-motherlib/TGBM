#include <tgbm/net/http_client.hpp>
#include <tgbm/net/errors.hpp>

namespace tgbm {

[[noreturn]] static void throw_bad_status(int status) {
  assert(status < 0);
  using enum reqerr_e::values;
  switch (reqerr_e::values(status)) {
    case done:
      unreachable();
    case timeout:
      throw timeout_exception{};
    case network_err:
      throw network_exception{""};
    case protocol_err:
      throw protocol_error{};
    case cancelled:
      throw std::runtime_error("HTTP client: request was canceled");
    case server_cancelled_request:
      throw std::runtime_error("HTTP client: request was canceled by server");
    default:
    case user_exception:
    case unknown_err:
      throw std::runtime_error("HTTP client unknown error happens");
  }
}

void handle_telegram_http_status(int status) {
  if (status < 0)
    throw_bad_status(status);
  if (status != 200) {
    if (status == 404)
      throw bad_request({});
    else
      throw http_exception(status);
  }
}

http_client::http_client(std::string_view host) : host(host) {
  if (host.empty() || host.starts_with("https://") || host.starts_with("http://"))
    throw std::invalid_argument("host should not be full url, correct example: \"api.telegram.org\"");
}

dd::task<http_response> http_client::send_request(http_request request, deadline_t deadline) {
  http_response rsp;
  auto on_header = [&](std::string_view name, std::string_view value) {
    rsp.headers.emplace_back(std::string(name), std::string(value));
  };
  auto on_data_part = [&](std::span<const byte_t> bytes, bool last_part) {
    rsp.body.insert(rsp.body.end(), bytes.begin(), bytes.end());
  };
  rsp.status = co_await send_request(&on_header, &on_data_part, std::move(request), deadline);
  if (rsp.status < 0)
    throw_bad_status(rsp.status);

  co_return std::move(rsp);
}

std::string_view e2str(reqerr_e::values e) noexcept {
  using enum reqerr_e::values;
  switch (e) {
    case done:
      return "done";
    case cancelled:
      return "cancelled";
    case user_exception:
      return "user_exception";
    case timeout:
      return "timeout";
    case network_err:
      return "network_err";
    case protocol_err:
      return "protocol_err";
    case server_cancelled_request:
      return "server_cancelled_request";
    case unknown_err:
      return "unknown_err";
    default:
      unreachable();  // error
  }
}

std::string_view e2str(http_method_e e) noexcept {
  using enum http_method_e;
  switch (e) {
    case GET:
      return "GET";
    case POST:
      return "POST";
    case PUT:
      return "PUT";
    case DELETE_:
      return "DELETE";
    case PATCH:
      return "PATCH";
    case OPTIONS:
      return "OPTIONS";
    case HEAD:
      return "HEAD";
    case CONNECT:
      return "CONNECT";
    case TRACE:
      return "TRACE";
    case UNKNOWN:
      return "UNKNOWN";
    default:
      unreachable();  // error
  }
}

void enum_from_string(std::string_view str, http_method_e& e) noexcept {
  using enum http_method_e;
  e = string_switch<http_method_e>(str)
          .case_("GET", GET)
          .case_("POST", POST)
          .case_("PUT", PUT)
          .case_("DELETE", DELETE_)
          .case_("PATCH", PATCH)
          .case_("OPTIONS", OPTIONS)
          .case_("TRACE", TRACE)
          .or_default(UNKNOWN);
}

std::string_view e2str(scheme_e e) noexcept {
  switch (e) {
    case scheme_e::HTTP:
      return "HTTP";
    case scheme_e::HTTPS:
      return "HTTPS";
    case scheme_e::UNKNOWN:
      return "UNKNOWN";
    default:
      unreachable();  // error
  }
}

void enum_from_string(std::string_view str, scheme_e& s) noexcept {
  using enum scheme_e;
  s = string_switch<scheme_e>(str).case_("HTTP", HTTP).case_("HTTPS", HTTPS).or_default(UNKNOWN);
}

}  // namespace tgbm
