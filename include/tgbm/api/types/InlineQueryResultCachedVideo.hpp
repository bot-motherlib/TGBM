#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*This object represents one result of an inline query. Telegram clients currently support results of the
 * following 20 types:*/
struct InlineQueryResultCachedVideo {
  /* Unique identifier for this result, 1-64 bytes */
  String id;
  /* A valid file identifier for the video file */
  String video_file_id;
  /* Title for the result */
  String title;
  /* Optional. Short description of the result */
  optional<String> description;
  /* Optional. Caption of the video to be sent, 0-1024 characters after entities parsing */
  optional<String> caption;
  /* Optional. Mode for parsing entities in the video caption. See formatting options for more details. */
  optional<String> parse_mode;
  /* Optional. List of special entities that appear in the caption, which can be specified instead of
   * parse_mode */
  optional<arrayof<MessageEntity>> caption_entities;
  /* Optional. Inline keyboard attached to the message */
  box<InlineKeyboardMarkup> reply_markup;
  /* Optional. Content of the message to be sent instead of the video */
  box<InputMessageContent> input_message_content;
  /* Optional. Pass True, if the caption must be shown above the message media */
  optional<bool> show_caption_above_media;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name)
        .case_("id", true)
        .case_("video_file_id", true)
        .case_("title", true)
        .or_default(false);
  }
};

}  // namespace tgbm::api
