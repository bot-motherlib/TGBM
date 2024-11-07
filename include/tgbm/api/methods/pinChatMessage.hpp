#pragma once

#include "tgbm/api/common.hpp"

namespace tgbm::api {

struct pin_chat_message_request {
  /* Unique identifier for the target chat or username of the target channel (in the format @channelusername)
   */
  int_or_str chat_id;
  /* Identifier of a message to pin */
  Integer message_id;
  /* Unique identifier of the business connection on behalf of which the message will be pinned */
  optional<String> business_connection_id;
  /* Pass True if it is not necessary to send a notification to all chat members about the new pinned message.
   * Notifications are always disabled in channels and private chats. */
  optional<bool> disable_notification;

  using return_type = bool;
  static constexpr file_info_e file_info = file_info_e::no;
  static constexpr std::string_view api_method_name = "pinChatMessage";
  static constexpr http_method_e http_method = http_method_e::POST;

  void fill_nonfile_args(auto& body) const {
    if (business_connection_id)
      body.arg("business_connection_id", *business_connection_id);
    body.arg("chat_id", chat_id);
    body.arg("message_id", message_id);
    if (disable_notification)
      body.arg("disable_notification", *disable_notification);
  }
};

}  // namespace tgbm::api
