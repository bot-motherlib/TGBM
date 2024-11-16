#pragma once

#include <tgbm/api/common.hpp>

namespace tgbm::api {

struct answer_pre_checkout_query_request {
  /* Unique identifier for the query to be answered */
  String pre_checkout_query_id;
  /* Specify True if everything is alright (goods are available, etc.) and the bot is ready to proceed with
   * the order. Use False if there are any problems. */
  bool ok;
  /* Required if ok is False. Error message in human readable form that explains the reason for failure to
   * proceed with the checkout (e.g. "Sorry, somebody just bought the last of our amazing black T-shirts while
   * you were busy filling out your payment details. Please choose a different color or garment!"). Telegram
   * will display this message to the user. */
  optional<String> error_message;

  using return_type = bool;
  static constexpr file_info_e file_info = file_info_e::no;
  static constexpr std::string_view api_method_name = "answerPreCheckoutQuery";
  static constexpr http_method_e http_method = http_method_e::POST;

  void fill_nonfile_args(auto& body) const {
    body.arg("pre_checkout_query_id", pre_checkout_query_id);
    body.arg("ok", ok);
    if (error_message)
      body.arg("error_message", *error_message);
  }
};

}  // namespace tgbm::api
