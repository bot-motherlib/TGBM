#pragma once

#include "tgbm/api/common.hpp"
#include "tgbm/api/types/InlineKeyboardMarkup.hpp"
#include "tgbm/api/types/Message.hpp"
#include "tgbm/api/types/MessageEntity.hpp"

namespace tgbm::api {

struct edit_message_caption_request {
  /* Unique identifier of the business connection on behalf of which the message to be edited was sent */
  optional<String> business_connection_id;
  /* Required if inline_message_id is not specified. Unique identifier for the target chat or username of the
   * target channel (in the format @channelusername) */
  optional<int_or_str> chat_id;
  /* Required if inline_message_id is not specified. Identifier of the message to edit */
  optional<Integer> message_id;
  /* Required if chat_id and message_id are not specified. Identifier of the inline message */
  optional<String> inline_message_id;
  /* New caption of the message, 0-1024 characters after entities parsing */
  optional<String> caption;
  /* Mode for parsing entities in the message caption. See formatting options for more details. */
  optional<String> parse_mode;
  /* A JSON-serialized list of special entities that appear in the caption, which can be specified instead of
   * parse_mode */
  optional<arrayof<MessageEntity>> caption_entities;
  /* Pass True, if the caption must be shown above the message media. Supported only for animation, photo and
   * video messages. */
  optional<bool> show_caption_above_media;
  /* A JSON-serialized object for an inline keyboard. */
  box<InlineKeyboardMarkup> reply_markup;

  using return_type = oneof<bool, Message>;
  static constexpr file_info_e file_info = file_info_e::no;
  static constexpr std::string_view api_method_name = "editMessageCaption";
  static constexpr http_method_e http_method = http_method_e::POST;

  void fill_nonfile_args(auto& body) const {
    if (business_connection_id)
      body.arg("business_connection_id", *business_connection_id);
    if (chat_id)
      body.arg("chat_id", *chat_id);
    if (message_id)
      body.arg("message_id", *message_id);
    if (inline_message_id)
      body.arg("inline_message_id", *inline_message_id);
    if (caption)
      body.arg("caption", *caption);
    if (parse_mode)
      body.arg("parse_mode", *parse_mode);
    if (caption_entities)
      body.arg("caption_entities", *caption_entities);
    if (show_caption_above_media)
      body.arg("show_caption_above_media", *show_caption_above_media);
    if (reply_markup)
      body.arg("reply_markup", *reply_markup);
  }
};

}  // namespace tgbm::api
