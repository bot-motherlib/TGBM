
#include <tgbm/net/http_base.hpp>
#include <tgbm/net/errors.hpp>
#include <tgbm/utils/string_switch.hpp>

namespace tgbm {

[[noreturn]] static void throw_bad_status(int status) {
  assert(status < 0);
  using enum http2::reqerr_e::values_e;
  switch (reqerr_e::values_e(status)) {
    case DONE:
      unreachable();
    case TIMEOUT:
      throw timeout_exception{};
    case NETWORK_ERR:
      throw network_exception{""};
    case PROTOCOL_ERR:
      throw http2::protocol_error{};
    case CANCELLED:
      throw std::runtime_error("HTTP client: request was canceled");
    case SERVER_CANCELLED_REQUEST:
      throw std::runtime_error("HTTP client: request was canceled by server");
    default:
    case UNKNOWN_ERR:
      throw std::runtime_error("HTTP client unknown error happens");
  }
}

void handle_telegram_http_status(int status) {
  if (status < 0)
    throw_bad_status(status);
  if (status != 200) {
    throw http_exception(status);
  }
}

}  // namespace tgbm
