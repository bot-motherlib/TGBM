#pragma once

#include <tgbm/api/common.hpp>
#include <tgbm/api/types/InlineKeyboardMarkup.hpp>
#include <tgbm/api/types/InputChecklist.hpp>
#include <tgbm/api/types/Message.hpp>
#include <tgbm/api/types/ReplyParameters.hpp>

namespace tgbm::api {

struct send_checklist_request {
  /* Unique identifier of the business connection on behalf of which the message will be sent */
  String business_connection_id;
  /* Unique identifier for the target chat */
  Integer chat_id;
  /* A JSON-serialized object for the checklist to send */
  box<InputChecklist> checklist;
  /* Sends the message silently. Users will receive a notification with no sound. */
  optional<bool> disable_notification;
  /* Protects the contents of the sent message from forwarding and saving */
  optional<bool> protect_content;
  /* Unique identifier of the message effect to be added to the message */
  optional<String> message_effect_id;
  /* A JSON-serialized object for description of the message to reply to */
  box<ReplyParameters> reply_parameters;
  /* A JSON-serialized object for an inline keyboard */
  box<InlineKeyboardMarkup> reply_markup;

  using return_type = Message;
  static constexpr file_info_e file_info = file_info_e::no;
  static constexpr std::string_view api_method_name = "sendChecklist";
  static constexpr http_method_e http_method = http_method_e::POST;

  void fill_nonfile_args(auto& body) const {
    body.arg("business_connection_id", business_connection_id);
    body.arg("chat_id", chat_id);
    body.arg("checklist", checklist);
    if (disable_notification)
      body.arg("disable_notification", *disable_notification);
    if (protect_content)
      body.arg("protect_content", *protect_content);
    if (message_effect_id)
      body.arg("message_effect_id", *message_effect_id);
    if (reply_parameters)
      body.arg("reply_parameters", *reply_parameters);
    if (reply_markup)
      body.arg("reply_markup", *reply_markup);
  }
};

}  // namespace tgbm::api
