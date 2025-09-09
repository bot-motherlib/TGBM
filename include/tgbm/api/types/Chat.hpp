#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*This object represents a chat.*/
struct Chat {
  /* Unique identifier for this chat. This number may have more than 32 significant bits and some programming
   * languages may have difficulty/silent defects in interpreting it. But it has at most 52 significant bits,
   * so a signed 64-bit integer or double-precision float type are safe for storing this identifier. */
  Integer id;
  /* Type of the chat, can be either “private”, “group”, “supergroup” or “channel” */
  String type;
  /* Optional. Title, for supergroups, channels and group chats */
  optional<String> title;
  /* Optional. Username, for private chats, supergroups and channels if available */
  optional<String> username;
  /* Optional. First name of the other party in a private chat */
  optional<String> first_name;
  /* Optional. Last name of the other party in a private chat */
  optional<String> last_name;
  /* Optional. True, if the supergroup chat is a forum (has topics enabled) */
  optional<True> is_forum;
  /* Optional. True, if the chat is the direct messages chat of a channel */
  optional<True> is_direct_messages;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("id", true).case_("type", true).or_default(false);
  }
};

}  // namespace tgbm::api
