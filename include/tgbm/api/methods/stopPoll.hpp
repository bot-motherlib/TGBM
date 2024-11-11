#pragma once

#include "tgbm/api/common.hpp"
#include "tgbm/api/types/InlineKeyboardMarkup.hpp"
#include "tgbm/api/types/Poll.hpp"

namespace tgbm::api {

struct stop_poll_request {
  /* Unique identifier for the target chat or username of the target channel (in the format @channelusername)
   */
  int_or_str chat_id;
  /* Identifier of the original message with the poll */
  Integer message_id;
  /* Unique identifier of the business connection on behalf of which the message to be edited was sent */
  optional<String> business_connection_id;
  /* A JSON-serialized object for a new message inline keyboard. */
  box<InlineKeyboardMarkup> reply_markup;

  using return_type = Poll;
  static constexpr file_info_e file_info = file_info_e::no;
  static constexpr std::string_view api_method_name = "stopPoll";
  static constexpr http_method_e http_method = http_method_e::POST;

  void fill_nonfile_args(auto& body) const {
    if (business_connection_id)
      body.arg("business_connection_id", *business_connection_id);
    body.arg("chat_id", chat_id);
    body.arg("message_id", message_id);
    if (reply_markup)
      body.arg("reply_markup", *reply_markup);
  }
};

}  // namespace tgbm::api
