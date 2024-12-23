#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*This object represents the content of a message to be sent as a result of an inline query. Telegram clients
 * currently support the following 5 types:*/
struct InputTextMessageContent {
  /* Optional. Mode for parsing entities in the message text. See formatting options for more details. */
  optional<String> parse_mode;
  /* Optional. List of special entities that appear in message text, which can be specified instead of
   * parse_mode */
  optional<arrayof<MessageEntity>> entities;
  /* Optional. Link preview generation options for the message */
  box<LinkPreviewOptions> link_preview_options;

  constexpr static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).or_default(false);
  }
};

}  // namespace tgbm::api
