#pragma once

#include <tgbm/api/common.hpp>
#include <tgbm/api/types/MessageEntity.hpp>

namespace tgbm::api {

struct gift_premium_subscription_request {
  /* Unique identifier of the target user who will receive a Telegram Premium subscription */
  Integer user_id;
  /* Number of months the Telegram Premium subscription will be active for the user; must be one of 3, 6, or
   * 12 */
  Integer month_count;
  /* Number of Telegram Stars to pay for the Telegram Premium subscription; must be 1000 for 3 months, 1500
   * for 6 months, and 2500 for 12 months */
  Integer star_count;
  /* Text that will be shown along with the service message about the subscription; 0-128 characters */
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
  static constexpr std::string_view api_method_name = "giftPremiumSubscription";
  static constexpr http_method_e http_method = http_method_e::POST;

  void fill_nonfile_args(auto& body) const {
    body.arg("user_id", user_id);
    body.arg("month_count", month_count);
    body.arg("star_count", star_count);
    if (text)
      body.arg("text", *text);
    if (text_parse_mode)
      body.arg("text_parse_mode", *text_parse_mode);
    if (text_entities)
      body.arg("text_entities", *text_entities);
  }
};

}  // namespace tgbm::api
