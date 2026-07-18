#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*Represents a voice message file to be sent.*/
struct InputMediaVoiceNote {
  /* Type of the media, must be voice_note */
  String type;
  /* File to send. Pass a file_id to send a file that exists on the Telegram servers (recommended), pass an
   * HTTP URL for Telegram to get a file from the Internet, or pass "attach://<file_attach_name>" to upload a
   * new one using multipart/form-data under <file_attach_name> name. More information on Sending Files » */
  String media;
  /* Optional. Caption of the voice message to be sent, 0-1024 characters after entities parsing */
  optional<String> caption;
  /* Optional. Mode for parsing entities in the voice message caption. See formatting options for more
   * details. */
  optional<String> parse_mode;
  /* Optional. List of special entities that appear in the caption, which can be specified instead of
   * parse_mode */
  optional<arrayof<MessageEntity>> caption_entities;
  /* Optional. Duration of the voice message in seconds */
  optional<Integer> duration;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("type", true).case_("media", true).or_default(false);
  }
};

}  // namespace tgbm::api
