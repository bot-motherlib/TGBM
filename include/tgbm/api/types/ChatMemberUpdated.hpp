#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*This object represents changes in the status of a chat member.*/
struct ChatMemberUpdated {
  /* Chat the user belongs to */
  box<Chat> chat;
  /* Performer of the action, which resulted in the change */
  box<User> from;
  /* Date the change was done in Unix time */
  Integer date;
  /* Previous information about the chat member */
  box<ChatMember> old_chat_member;
  /* New information about the chat member */
  box<ChatMember> new_chat_member;
  /* Optional. Chat invite link, which was used by the user to join the chat; for joining by invite link
   * events only. */
  box<ChatInviteLink> invite_link;
  /* Optional. True, if the user joined the chat after sending a direct join request without using an invite
   * link and being approved by an administrator */
  optional<bool> via_join_request;
  /* Optional. True, if the user joined the chat via a chat folder invite link */
  optional<bool> via_chat_folder_invite_link;

  constexpr static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name)
        .case_("chat", true)
        .case_("from", true)
        .case_("date", true)
        .case_("old_chat_member", true)
        .case_("new_chat_member", true)
        .or_default(false);
  }
};

}  // namespace tgbm::api
