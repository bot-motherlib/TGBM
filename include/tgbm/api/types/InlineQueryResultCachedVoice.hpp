#pragma once

#include "all_fwd.hpp"

namespace tgbm::api {

/*This object represents one result of an inline query. Telegram clients currently support results of the
 * following 20 types:*/
struct InlineQueryResultCachedVoice {
  /* Unique identifier for this result, 1-64 bytes */
  String id;
  /* A valid file identifier for the voice message */
  String voice_file_id;
  /* Voice message title */
  String title;
  /* Optional. Caption, 0-1024 characters after entities parsing */
  optional<String> caption;
  /* Optional. Mode for parsing entities in the voice message caption. See formatting options for more
   * details. */
  optional<String> parse_mode;
  /* Optional. List of special entities that appear in the caption, which can be specified instead of
   * parse_mode */
  optional<arrayof<MessageEntity>> caption_entities;
  /* Optional. Inline keyboard attached to the message */
  box<InlineKeyboardMarkup> reply_markup;
  /* Optional. Content of the message to be sent instead of the voice message */
  box<InputMessageContent> input_message_content;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name)
        .case_("id", true)
        .case_("voice_file_id", true)
        .case_("title", true)
        .or_default(false);
  }
};

}  // namespace tgbm::api
