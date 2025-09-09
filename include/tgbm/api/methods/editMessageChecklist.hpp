#pragma once

#include <tgbm/api/common.hpp>
#include <tgbm/api/types/InlineKeyboardMarkup.hpp>
#include <tgbm/api/types/InputChecklist.hpp>
#include <tgbm/api/types/Message.hpp>

namespace tgbm::api {

struct edit_message_checklist_request {
  /* Unique identifier of the business connection on behalf of which the message will be sent */
  String business_connection_id;
  /* Unique identifier for the target chat */
  Integer chat_id;
  /* Unique identifier for the target message */
  Integer message_id;
  /* A JSON-serialized object for the new checklist */
  box<InputChecklist> checklist;
  /* A JSON-serialized object for the new inline keyboard for the message */
  box<InlineKeyboardMarkup> reply_markup;

  using return_type = Message;
  static constexpr file_info_e file_info = file_info_e::no;
  static constexpr std::string_view api_method_name = "editMessageChecklist";
  static constexpr http_method_e http_method = http_method_e::POST;

  void fill_nonfile_args(auto& body) const {
    body.arg("business_connection_id", business_connection_id);
    body.arg("chat_id", chat_id);
    body.arg("message_id", message_id);
    body.arg("checklist", checklist);
    if (reply_markup)
      body.arg("reply_markup", *reply_markup);
  }
};

}  // namespace tgbm::api
