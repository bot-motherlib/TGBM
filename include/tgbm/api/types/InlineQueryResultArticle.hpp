#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*This object represents one result of an inline query. Telegram clients currently support results of the
 * following 20 types:*/
struct InlineQueryResultArticle {
  /* Unique identifier for this result, 1-64 Bytes */
  String id;
  /* Title of the result */
  String title;
  /* Content of the message to be sent */
  box<InputMessageContent> input_message_content;
  /* Optional. Inline keyboard attached to the message */
  box<InlineKeyboardMarkup> reply_markup;
  /* Optional. URL of the result */
  optional<String> url;
  /* Optional. Short description of the result */
  optional<String> description;
  /* Optional. Url of the thumbnail for the result */
  optional<String> thumbnail_url;
  /* Optional. Thumbnail width */
  optional<Integer> thumbnail_width;
  /* Optional. Thumbnail height */
  optional<Integer> thumbnail_height;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name)
        .case_("id", true)
        .case_("title", true)
        .case_("input_message_content", true)
        .or_default(false);
  }
};

}  // namespace tgbm::api
