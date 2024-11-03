#pragma once

#include <concepts>

#include <kelcoro/task.hpp>
#include <kelcoro/channel.hpp>

#include "tgbm/api/common.hpp"
#include "tgbm/net/errors.hpp"
#include "tgbm/net/http_client.hpp"
#include "tgbm/tools/deadline.hpp"
#include "tgbm/tools/memory.hpp"
#include "tgbm/api/telegram_answer.hpp"
#include "tgbm/tools/json_tools/stream_parser.hpp"

namespace tgbm::api {

namespace noexport {

template <typename T>
concept tgapi_request_decayed = requires {
  { T::file_info } -> std::convertible_to<file_info_e>;
  { T::api_method_name } -> std::convertible_to<std::string_view>;
  { T::http_method } -> std::convertible_to<http_method_e>;
  typename T::return_type;
};

}  // namespace noexport

template <typename T>
concept tgapi_request = noexport::tgapi_request_decayed<std::decay_t<T>>;

template <typename T>
concept tgapi_file_request =
    tgapi_request<T> && std::same_as<typename std::decay_t<T>::return_type, InputFile>;

template <tgapi_request R>
using request_return_t = typename R::return_type;

using default_http_body = application_json_body;

// TODO tests for this function
template <tgapi_request R>
[[nodiscard]] http_body make_http_boby(const R& request) {
  static_assert(R::file_info == file_info_e::no || R::file_info == file_info_e::yes ||
                R::file_info == file_info_e::maybe);
  if constexpr (R::http_method == http_method_e::GET) {
    return default_http_body{}.take();
  } else if constexpr (R::file_info == file_info_e::no) {
    default_http_body body;
    request.fill_nonfile_args(body);
    return body.take();
  } else if constexpr (R::file_info == file_info_e::yes) {
    application_multipart_form_data body;
    request.fill_nonfile_args(body);
    request.fill_file_args(body);
    return body.take();
  } else if constexpr (R::file_info == file_info_e::maybe) {
    if (std::is_same_v<default_http_body, application_multipart_form_data> || request.has_file_args()) {
      application_multipart_form_data body;
      request.fill_nonfile_args(body);
      request.fill_file_args(body);
      return body.take();
    } else {
      default_http_body body;
      request.fill_nonfile_args(body);
      return body.take();
    }
  }
}

template <tgapi_request R>
[[nodiscard]] http_request make_request(const R& r, std::string_view bottoken) {
  std::string path;
  path.reserve(bottoken.size() + R::api_method_name.size() + 5 /* /bot/ */);
  path += "/bot";
  path += bottoken;
  path.push_back('/');
  path += R::api_method_name;
  return http_request{
      .authority = {},
      .path = std::move(path),
      .method = R::http_method,
      .scheme = scheme_e::HTTPS,
      .body = make_http_boby(r),
  };
}

struct reqerr_t {
  int status = 0;  // if < 0 then reqerr_e, > 0 status from telegram, 0 means 'done' (when ignores result)
  const_string description;  // description, sended by telegram

  [[nodiscard]] bool successfully_sended() const noexcept {
    return status >= 0;
  }
  [[nodiscard]] bool successfully_received() const noexcept {
    // TODO check telegram always sends 200 if "ok": true
    return status == 200;
  }

  // returns true if error happened
  explicit operator bool() const noexcept {
    return !successfully_sended() || !successfully_received();
  }
};

// Note: 'out' must be alive while coroutine working
template <tgapi_request R>
  requires(!tgapi_file_request<R>)
dd::task<reqerr_t> send_request(const R& request, http_client& client, const_string bottoken,
                                request_return_t<R>& out, duration_t timeout) {
  telegram_answer r(out);
  json::stream_parser parser(r);

  auto parse_to_out = [&, ec = io_error_code{}](std::span<byte_t> bytes, bool last_part) mutable {
    parser.feed(std::string_view((const char*)bytes.data(), bytes.size()), last_part, ec);
    if (ec)
      throw json::parse_error(ec.what());
  };
  int status =
      co_await client.send_request(nullptr, &parse_to_out, make_request(request, bottoken.str()), timeout);
  co_return reqerr_t{.status = status, .description = std::move(r.descripion)};
}

// ignores result, but waits telegram answer for status
// returns http status or reqerr_e (< 0)
template <tgapi_request R>
dd::task<int> send_request(const R& request, http_client& client, const_string bottoken, duration_t timeout) {
  auto noop = [](std::string_view, std::string_view) {};
  return client.send_request(&noop, nullptr, make_request(request, bottoken.str()), timeout);
}

// for files
// Note: on_data_part ptr must be alive while coroutine works
// returns http status or reqerr_e (< 0)
template <tgapi_file_request R>
dd::task<int> send_request(const R& request, http_client& client, const_string bottoken,
                           on_data_part_fn_ref on_data_part, duration_t timeout) {
  return client.send_request(nullptr, &on_data_part, make_request(request, bottoken.str()), timeout);
}

}  // namespace tgbm::api
