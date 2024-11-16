#pragma once

#include <tgbm/api/common.hpp>

namespace tgbm::api {

struct answer_callback_query_request {
  /* Unique identifier for the query to be answered */
  String callback_query_id;
  /* Text of the notification. If not specified, nothing will be shown to the user, 0-200 characters */
  optional<String> text;
  /* If True, an alert will be shown by the client instead of a notification at the top of the chat screen.
   * Defaults to false. */
  optional<bool> show_alert;
  /* URL that will be opened by the user's client. If you have created a Game and accepted the conditions via
   * @BotFather, specify the URL that opens your game - note that this will only work if the query comes from
   * a callback_game button.Otherwise, you may use links like t.me/your_bot?start=XXXX that open your bot with
   * a parameter. */
  optional<String> url;
  /* The maximum amount of time in seconds that the result of the callback query may be cached client-side.
   * Telegram apps will support caching starting in version 3.14. Defaults to 0. */
  optional<Integer> cache_time;

  using return_type = bool;
  static constexpr file_info_e file_info = file_info_e::no;
  static constexpr std::string_view api_method_name = "answerCallbackQuery";
  static constexpr http_method_e http_method = http_method_e::POST;

  void fill_nonfile_args(auto& body) const {
    body.arg("callback_query_id", callback_query_id);
    if (text)
      body.arg("text", *text);
    if (show_alert)
      body.arg("show_alert", *show_alert);
    if (url)
      body.arg("url", *url);
    if (cache_time)
      body.arg("cache_time", *cache_time);
  }
};

}  // namespace tgbm::api
