#pragma once

#include "tgbm/api/common.hpp"

namespace tgbm::api {

struct unpin_chat_message_request {
  /* Unique identifier for the target chat or username of the target channel (in the format @channelusername)
   */
  int_or_str chat_id;
  /* Unique identifier of the business connection on behalf of which the message will be unpinned */
  optional<String> business_connection_id;
  /* Identifier of the message to unpin. Required if business_connection_id is specified. If not specified,
   * the most recent pinned message (by sending date) will be unpinned. */
  optional<Integer> message_id;

  using return_type = bool;
  static constexpr file_info_e file_info = file_info_e::no;
  static constexpr std::string_view api_method_name = "unpinChatMessage";
  static constexpr http_method_e http_method = http_method_e::POST;

  void fill_nonfile_args(auto& body) const {
    if (business_connection_id)
      body.arg("business_connection_id", *business_connection_id);
    body.arg("chat_id", chat_id);
    if (message_id)
      body.arg("message_id", *message_id);
  }
};

}  // namespace tgbm::api
