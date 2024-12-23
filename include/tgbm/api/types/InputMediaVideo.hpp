#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*This object represents the content of a media message to be sent. It should be one of*/
struct InputMediaVideo {
  /* File to send. Pass a file_id to send a file that exists on the Telegram servers (recommended), pass an
   * HTTP URL for Telegram to get a file from the Internet, or pass “attach://<file_attach_name>” to upload a
   * new one using multipart/form-data under <file_attach_name> name. More information on Sending Files » */
  String media;
  /* Optional. Thumbnail of the file sent; can be ignored if thumbnail generation for the file is supported
   * server-side. The thumbnail should be in JPEG format and less than 200 kB in size. A thumbnail's width and
   * height should not exceed 320. Ignored if the file is not uploaded using multipart/form-data. Thumbnails
   * can't be reused and can be only uploaded as a new file, so you can pass “attach://<file_attach_name>” if
   * the thumbnail was uploaded using multipart/form-data under <file_attach_name>. More information on
   * Sending Files » */
  optional<file_or_str> thumbnail;
  /* Optional. Caption of the video to be sent, 0-1024 characters after entities parsing */
  optional<String> caption;
  /* Optional. Mode for parsing entities in the video caption. See formatting options for more details. */
  optional<String> parse_mode;
  /* Optional. List of special entities that appear in the caption, which can be specified instead of
   * parse_mode */
  optional<arrayof<MessageEntity>> caption_entities;
  /* Optional. Video width */
  optional<Integer> width;
  /* Optional. Video height */
  optional<Integer> height;
  /* Optional. Video duration in seconds */
  optional<Integer> duration;
  /* Optional. Pass True, if the caption must be shown above the message media */
  optional<bool> show_caption_above_media;
  /* Optional. Pass True if the uploaded video is suitable for streaming */
  optional<bool> supports_streaming;
  /* Optional. Pass True if the video needs to be covered with a spoiler animation */
  optional<bool> has_spoiler;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("media", true).or_default(false);
  }
};

}  // namespace tgbm::api
