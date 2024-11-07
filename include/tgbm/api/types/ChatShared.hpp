#pragma once

#include "all_fwd.hpp"

namespace tgbm::api {

/*This object contains information about a chat that was shared with the bot using a KeyboardButtonRequestChat
 * button.*/
struct ChatShared {
  /* Identifier of the request */
  Integer request_id;
  /* Identifier of the shared chat. This number may have more than 32 significant bits and some programming
   * languages may have difficulty/silent defects in interpreting it. But it has at most 52 significant bits,
   * so a 64-bit integer or double-precision float type are safe for storing this identifier. The bot may not
   * have access to the chat and could be unable to use this identifier, unless the chat is already known to
   * the bot by some other means. */
  Integer chat_id;
  /* Optional. Title of the chat, if the title was requested by the bot. */
  optional<String> title;
  /* Optional. Username of the chat, if the username was requested by the bot and available. */
  optional<String> username;
  /* Optional. Available sizes of the chat photo, if the photo was requested by the bot */
  optional<arrayof<PhotoSize>> photo;

  consteval static bool is_mandatory_field(std::string_view name) {
    return utils::string_switch<bool>(name)
        .case_("request_id", true)
        .case_("chat_id", true)
        .or_default(false);
  }
};

}  // namespace tgbm::api
