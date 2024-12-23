#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*This object represents the content of a media message to be sent. It should be one of*/
struct InputMediaPhoto {
  /* File to send. Pass a file_id to send a file that exists on the Telegram servers (recommended), pass an
   * HTTP URL for Telegram to get a file from the Internet, or pass “attach://<file_attach_name>” to upload a
   * new one using multipart/form-data under <file_attach_name> name. More information on Sending Files » */
  String media;
  /* Optional. Caption of the photo to be sent, 0-1024 characters after entities parsing */
  optional<String> caption;
  /* Optional. Mode for parsing entities in the photo caption. See formatting options for more details. */
  optional<String> parse_mode;
  /* Optional. List of special entities that appear in the caption, which can be specified instead of
   * parse_mode */
  optional<arrayof<MessageEntity>> caption_entities;
  /* Optional. Pass True, if the caption must be shown above the message media */
  optional<bool> show_caption_above_media;
  /* Optional. Pass True if the photo needs to be covered with a spoiler animation */
  optional<bool> has_spoiler;

  constexpr static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("media", true).or_default(false);
  }
};

}  // namespace tgbm::api
