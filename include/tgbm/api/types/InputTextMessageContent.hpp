#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*Represents the content of a text message to be sent as the result of an inline query.*/
struct InputTextMessageContent {
  /* Optional. Mode for parsing entities in the message text. See formatting options for more details. */
  optional<String> parse_mode;
  /* Optional. List of special entities that appear in message text, which can be specified instead of
   * parse_mode */
  optional<arrayof<MessageEntity>> entities;
  /* Optional. Link preview generation options for the message */
  box<LinkPreviewOptions> link_preview_options;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).or_default(false);
  }
};

}  // namespace tgbm::api
