#pragma once

#include "all_fwd.hpp"

namespace tgbm::api {

/*This object represents a Telegram user or bot.*/
struct User {
  /* Unique identifier for this user or bot. This number may have more than 32 significant bits and some
   * programming languages may have difficulty/silent defects in interpreting it. But it has at most 52
   * significant bits, so a 64-bit integer or double-precision float type are safe for storing this
   * identifier. */
  Integer id;
  /* True, if this user is a bot */
  bool is_bot;
  /* User's or bot's first name */
  String first_name;
  /* Optional. User's or bot's last name */
  optional<String> last_name;
  /* Optional. User's or bot's username */
  optional<String> username;
  /* Optional. IETF language tag of the user's language */
  optional<String> language_code;
  /* Optional. True, if this user is a Telegram Premium user */
  optional<True> is_premium;
  /* Optional. True, if this user added the bot to the attachment menu */
  optional<True> added_to_attachment_menu;
  /* Optional. True, if the bot can be invited to groups. Returned only in getMe. */
  optional<bool> can_join_groups;
  /* Optional. True, if privacy mode is disabled for the bot. Returned only in getMe. */
  optional<bool> can_read_all_group_messages;
  /* Optional. True, if the bot supports inline queries. Returned only in getMe. */
  optional<bool> supports_inline_queries;
  /* Optional. True, if the bot can be connected to a Telegram Business account to receive its messages.
   * Returned only in getMe. */
  optional<bool> can_connect_to_business;
  /* Optional. True, if the bot has a main Web App. Returned only in getMe. */
  optional<bool> has_main_web_app;

  consteval static bool is_mandatory_field(std::string_view name) {
    return utils::string_switch<bool>(name)
        .case_("id", true)
        .case_("is_bot", true)
        .case_("first_name", true)
        .or_default(false);
  }
};

}  // namespace tgbm::api
