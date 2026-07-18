#pragma once

#include <tgbm/api/common.hpp>
#include <tgbm/api/types/InlineKeyboardMarkup.hpp>
#include <tgbm/api/types/LinkPreviewOptions.hpp>
#include <tgbm/api/types/MessageEntity.hpp>

namespace tgbm::api {

struct edit_ephemeral_message_text_request {
  /* Unique identifier for the target chat or username of the target supergroup in the format @username */
  int_or_str chat_id;
  /* Identifier of the user who received the message */
  Integer receiver_user_id;
  /* Identifier of the ephemeral message to edit */
  Integer ephemeral_message_id;
  /* New text of the message, 1-4096 characters after entity parsing */
  String text;
  /* Mode for parsing entities in the message text. See formatting options for more details. */
  optional<String> parse_mode;
  /* A JSON-serialized list of special entities that appear in message text, which can be specified instead of
   * parse_mode */
  optional<arrayof<MessageEntity>> entities;
  /* Link preview generation options for the message */
  box<LinkPreviewOptions> link_preview_options;
  /* A JSON-serialized object for an inline keyboard */
  box<InlineKeyboardMarkup> reply_markup;

  using return_type = bool;
  static constexpr file_info_e file_info = file_info_e::no;
  static constexpr std::string_view api_method_name = "editEphemeralMessageText";
  static constexpr http_method_e http_method = http_method_e::POST;

  void fill_nonfile_args(auto& body) const {
    body.arg("chat_id", chat_id);
    body.arg("receiver_user_id", receiver_user_id);
    body.arg("ephemeral_message_id", ephemeral_message_id);
    body.arg("text", text);
    if (parse_mode)
      body.arg("parse_mode", *parse_mode);
    if (entities)
      body.arg("entities", *entities);
    if (link_preview_options)
      body.arg("link_preview_options", *link_preview_options);
    if (reply_markup)
      body.arg("reply_markup", *reply_markup);
  }
};

}  // namespace tgbm::api
