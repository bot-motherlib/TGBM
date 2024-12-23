#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*This object represents one result of an inline query. Telegram clients currently support results of the
 * following 20 types:*/
struct InlineQueryResultVoice {
  /* Unique identifier for this result, 1-64 bytes */
  String id;
  /* A valid URL for the voice recording */
  String voice_url;
  /* Recording title */
  String title;
  /* Optional. Caption, 0-1024 characters after entities parsing */
  optional<String> caption;
  /* Optional. Mode for parsing entities in the voice message caption. See formatting options for more
   * details. */
  optional<String> parse_mode;
  /* Optional. List of special entities that appear in the caption, which can be specified instead of
   * parse_mode */
  optional<arrayof<MessageEntity>> caption_entities;
  /* Optional. Recording duration in seconds */
  optional<Integer> voice_duration;
  /* Optional. Inline keyboard attached to the message */
  box<InlineKeyboardMarkup> reply_markup;
  /* Optional. Content of the message to be sent instead of the voice recording */
  box<InputMessageContent> input_message_content;

  constexpr static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name)
        .case_("id", true)
        .case_("voice_url", true)
        .case_("title", true)
        .or_default(false);
  }
};

}  // namespace tgbm::api
