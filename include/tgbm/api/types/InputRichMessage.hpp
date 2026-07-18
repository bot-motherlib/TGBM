#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*Describes a rich message to be sent. Exactly one of the fields html, markdown, or blocks must be used.*/
struct InputRichMessage {
  /* Optional. Content of the rich message to send described as a list of blocks */
  optional<arrayof<InputRichBlock>> blocks;
  /* Optional. Content of the rich message to send described using HTML formatting. See rich message
   * formatting options for more details. Use media field to specify the media used in the message. */
  optional<String> html;
  /* Optional. Content of the rich message to send described using Markdown formatting. See rich message
   * formatting options for more details. Use media field to specify the media used in the message. */
  optional<String> markdown;
  /* Optional. List of media that are specified in the markdown or html fields using tg://photo?id=,
   * tg://video?id=, and tg://audio?id= links */
  optional<arrayof<InputRichMessageMedia>> media;
  /* Optional. Pass True if the rich message must be shown right-to-left */
  optional<bool> is_rtl;
  /* Optional. Pass True to skip automatic detection of entities (e.g., URLs, email addresses, username
   * mentions, hashtags, cashtags, bot commands, or phone numbers) in the text */
  optional<bool> skip_entity_detection;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).or_default(false);
  }
};

}  // namespace tgbm::api
