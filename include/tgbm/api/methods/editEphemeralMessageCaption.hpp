#pragma once

#include <tgbm/api/common.hpp>
#include <tgbm/api/types/InlineKeyboardMarkup.hpp>
#include <tgbm/api/types/MessageEntity.hpp>

namespace tgbm::api {

struct edit_ephemeral_message_caption_request {
  /* Unique identifier for the target chat or username of the target supergroup in the format @username */
  int_or_str chat_id;
  /* Identifier of the user who received the message */
  Integer receiver_user_id;
  /* Identifier of the ephemeral message to edit */
  Integer ephemeral_message_id;
  /* New caption of the message, 0-1024 characters after entities parsing */
  optional<String> caption;
  /* Mode for parsing entities in the message caption. See formatting options for more details. */
  optional<String> parse_mode;
  /* A JSON-serialized list of special entities that appear in the caption, which can be specified instead of
   * parse_mode */
  optional<arrayof<MessageEntity>> caption_entities;
  /* A JSON-serialized object for an inline keyboard */
  box<InlineKeyboardMarkup> reply_markup;

  using return_type = bool;
  static constexpr file_info_e file_info = file_info_e::no;
  static constexpr std::string_view api_method_name = "editEphemeralMessageCaption";
  static constexpr http_method_e http_method = http_method_e::POST;

  void fill_nonfile_args(auto& body) const {
    body.arg("chat_id", chat_id);
    body.arg("receiver_user_id", receiver_user_id);
    body.arg("ephemeral_message_id", ephemeral_message_id);
    if (caption)
      body.arg("caption", *caption);
    if (parse_mode)
      body.arg("parse_mode", *parse_mode);
    if (caption_entities)
      body.arg("caption_entities", *caption_entities);
    if (reply_markup)
      body.arg("reply_markup", *reply_markup);
  }
};

}  // namespace tgbm::api
