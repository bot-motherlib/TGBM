#pragma once

#include "all_fwd.hpp"

namespace tgbm::api {

/*This object represents one result of an inline query. Telegram clients currently support results of the
 * following 20 types:*/
struct InlineQueryResultVideo {
  /* Unique identifier for this result, 1-64 bytes */
  String id;
  /* A valid URL for the embedded video player or video file */
  String video_url;
  /* MIME type of the content of the video URL, “text/html” or “video/mp4” */
  String mime_type;
  /* URL of the thumbnail (JPEG only) for the video */
  String thumbnail_url;
  /* Title for the result */
  String title;
  /* Optional. Caption of the video to be sent, 0-1024 characters after entities parsing */
  optional<String> caption;
  /* Optional. Mode for parsing entities in the video caption. See formatting options for more details. */
  optional<String> parse_mode;
  /* Optional. List of special entities that appear in the caption, which can be specified instead of
   * parse_mode */
  optional<arrayof<MessageEntity>> caption_entities;
  /* Optional. Video width */
  optional<Integer> video_width;
  /* Optional. Video height */
  optional<Integer> video_height;
  /* Optional. Video duration in seconds */
  optional<Integer> video_duration;
  /* Optional. Short description of the result */
  optional<String> description;
  /* Optional. Inline keyboard attached to the message */
  box<InlineKeyboardMarkup> reply_markup;
  /* Optional. Content of the message to be sent instead of the video. This field is required if
   * InlineQueryResultVideo is used to send an HTML-page as a result (e.g., a YouTube video). */
  box<InputMessageContent> input_message_content;
  /* Optional. Pass True, if the caption must be shown above the message media */
  optional<bool> show_caption_above_media;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name)
        .case_("id", true)
        .case_("video_url", true)
        .case_("mime_type", true)
        .case_("thumbnail_url", true)
        .case_("title", true)
        .or_default(false);
  }
};

}  // namespace tgbm::api
