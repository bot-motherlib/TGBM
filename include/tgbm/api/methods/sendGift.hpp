#pragma once

#include <tgbm/api/common.hpp>
#include <tgbm/api/types/MessageEntity.hpp>

namespace tgbm::api {

struct send_gift_request {
  /* Identifier of the gift */
  String gift_id;
  /* Required if chat_id is not specified. Unique identifier of the target user who will receive the gift. */
  optional<Integer> user_id;
  /* Required if user_id is not specified. Unique identifier for the chat or username of the channel (in the
   * format @channelusername) that will receive the gift. */
  optional<int_or_str> chat_id;
  /* Pass True to pay for the gift upgrade from the bot's balance, thereby making the upgrade free for the
   * receiver */
  optional<bool> pay_for_upgrade;
  /* Text that will be shown along with the gift; 0-128 characters */
  optional<String> text;
  /* Mode for parsing entities in the text. See formatting options for more details. Entities other than
   * “bold”, “italic”, “underline”, “strikethrough”, “spoiler”, and “custom_emoji” are ignored. */
  optional<String> text_parse_mode;
  /* A JSON-serialized list of special entities that appear in the gift text. It can be specified instead of
   * text_parse_mode. Entities other than “bold”, “italic”, “underline”, “strikethrough”, “spoiler”, and
   * “custom_emoji” are ignored. */
  optional<arrayof<MessageEntity>> text_entities;

  using return_type = bool;
  static constexpr file_info_e file_info = file_info_e::no;
  static constexpr std::string_view api_method_name = "sendGift";
  static constexpr http_method_e http_method = http_method_e::POST;

  void fill_nonfile_args(auto& body) const {
    if (user_id)
      body.arg("user_id", *user_id);
    if (chat_id)
      body.arg("chat_id", *chat_id);
    body.arg("gift_id", gift_id);
    if (pay_for_upgrade)
      body.arg("pay_for_upgrade", *pay_for_upgrade);
    if (text)
      body.arg("text", *text);
    if (text_parse_mode)
      body.arg("text_parse_mode", *text_parse_mode);
    if (text_entities)
      body.arg("text_entities", *text_entities);
  }
};

}  // namespace tgbm::api
