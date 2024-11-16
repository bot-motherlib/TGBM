#pragma once

#include "all_fwd.hpp"

namespace tgbm::api {

/*Describes the connection of the bot with a business account.*/
struct BusinessConnection {
  /* Unique identifier of the business connection */
  String id;
  /* Business account user that created the business connection */
  box<User> user;
  /* Identifier of a private chat with the user who created the business connection. This number may have more
   * than 32 significant bits and some programming languages may have difficulty/silent defects in
   * interpreting it. But it has at most 52 significant bits, so a 64-bit integer or double-precision float
   * type are safe for storing this identifier. */
  Integer user_chat_id;
  /* Date the connection was established in Unix time */
  Integer date;
  /* True, if the bot can act on behalf of the business account in chats that were active in the last 24 hours
   */
  bool can_reply;
  /* True, if the connection is active */
  bool is_enabled;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name)
        .case_("id", true)
        .case_("user", true)
        .case_("user_chat_id", true)
        .case_("date", true)
        .case_("can_reply", true)
        .case_("is_enabled", true)
        .or_default(false);
  }
};

}  // namespace tgbm::api
