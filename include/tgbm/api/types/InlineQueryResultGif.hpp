#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*This object represents one result of an inline query. Telegram clients currently support results of the
 * following 20 types:*/
struct InlineQueryResultGif {
  /* Unique identifier for this result, 1-64 bytes */
  String id;
  /* A valid URL for the GIF file. File size must not exceed 1MB */
  String gif_url;
  /* URL of the static (JPEG or GIF) or animated (MPEG4) thumbnail for the result */
  String thumbnail_url;
  /* Optional. Width of the GIF */
  optional<Integer> gif_width;
  /* Optional. Height of the GIF */
  optional<Integer> gif_height;
  /* Optional. Duration of the GIF in seconds */
  optional<Integer> gif_duration;
  /* Optional. MIME type of the thumbnail, must be one of “image/jpeg”, “image/gif”, or “video/mp4”. Defaults
   * to “image/jpeg” */
  optional<String> thumbnail_mime_type;
  /* Optional. Title for the result */
  optional<String> title;
  /* Optional. Caption of the GIF file to be sent, 0-1024 characters after entities parsing */
  optional<String> caption;
  /* Optional. Mode for parsing entities in the caption. See formatting options for more details. */
  optional<String> parse_mode;
  /* Optional. List of special entities that appear in the caption, which can be specified instead of
   * parse_mode */
  optional<arrayof<MessageEntity>> caption_entities;
  /* Optional. Inline keyboard attached to the message */
  box<InlineKeyboardMarkup> reply_markup;
  /* Optional. Content of the message to be sent instead of the GIF animation */
  box<InputMessageContent> input_message_content;
  /* Optional. Pass True, if the caption must be shown above the message media */
  optional<bool> show_caption_above_media;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name)
        .case_("id", true)
        .case_("gif_url", true)
        .case_("thumbnail_url", true)
        .or_default(false);
  }
};

}  // namespace tgbm::api
