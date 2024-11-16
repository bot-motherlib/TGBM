#pragma once

#include "all_fwd.hpp"

namespace tgbm::api {

/*Represents a join request sent to a chat.*/
struct ChatJoinRequest {
  /* Chat to which the request was sent */
  box<Chat> chat;
  /* User that sent the join request */
  box<User> from;
  /* Identifier of a private chat with the user who sent the join request. This number may have more than 32
   * significant bits and some programming languages may have difficulty/silent defects in interpreting it.
   * But it has at most 52 significant bits, so a 64-bit integer or double-precision float type are safe for
   * storing this identifier. The bot can use this identifier for 5 minutes to send messages until the join
   * request is processed, assuming no other administrator contacted the user. */
  Integer user_chat_id;
  /* Date the request was sent in Unix time */
  Integer date;
  /* Optional. Bio of the user. */
  optional<String> bio;
  /* Optional. Chat invite link that was used by the user to send the join request */
  box<ChatInviteLink> invite_link;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name)
        .case_("chat", true)
        .case_("from", true)
        .case_("user_chat_id", true)
        .case_("date", true)
        .or_default(false);
  }
};

}  // namespace tgbm::api
