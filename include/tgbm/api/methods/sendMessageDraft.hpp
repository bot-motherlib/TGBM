#pragma once

#include <tgbm/api/common.hpp>
#include <tgbm/api/types/MessageEntity.hpp>

namespace tgbm::api {

struct send_message_draft_request {
  /* Unique identifier for the target private chat */
  Integer chat_id;
  /* Unique identifier of the message draft; must be non-zero. Changes of drafts with the same identifier are
   * animated */
  Integer draft_id;
  /* Text of the message to be sent, 1-4096 characters after entities parsing */
  String text;
  /* Unique identifier for the target message thread */
  optional<Integer> message_thread_id;
  /* Mode for parsing entities in the message text. See formatting options for more details. */
  optional<String> parse_mode;
  /* A JSON-serialized list of special entities that appear in message text, which can be specified instead of
   * parse_mode */
  optional<arrayof<MessageEntity>> entities;

  using return_type = bool;
  static constexpr file_info_e file_info = file_info_e::no;
  static constexpr std::string_view api_method_name = "sendMessageDraft";
  static constexpr http_method_e http_method = http_method_e::POST;

  void fill_nonfile_args(auto& body) const {
    body.arg("chat_id", chat_id);
    if (message_thread_id)
      body.arg("message_thread_id", *message_thread_id);
    body.arg("draft_id", draft_id);
    body.arg("text", text);
    if (parse_mode)
      body.arg("parse_mode", *parse_mode);
    if (entities)
      body.arg("entities", *entities);
  }
};

}  // namespace tgbm::api
