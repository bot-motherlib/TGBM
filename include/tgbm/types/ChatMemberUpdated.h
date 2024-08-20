#pragma once

#include "tgbm/types/Chat.h"
#include "tgbm/types/User.h"
#include "tgbm/types/ChatMember.h"
#include "tgbm/types/ChatInviteLink.h"

#include <cstdint>
#include <memory>

namespace tgbm {
// TODO tojson/from причём from непонятно какого джсона, лучше бы не рапида for all types (automatic)
/**
 * @brief This object represents changes in the status of a chat member.
 *
 * @ingroup types
 */
class ChatMemberUpdated {
 public:
  typedef std::shared_ptr<ChatMemberUpdated> Ptr;

  /**
   * @brief Chat the user belongs to
   */
  Chat::Ptr chat;

  /**
   * @brief Performer of the action, which resulted in the change
   */
  User::Ptr from;

  /**
   * @brief Date the change was done in Unix time
   */
  std::uint32_t date;

  /**
   * @brief Previous information about the chat member
   */
  ChatMember::Ptr oldChatMember;

  /**
   * @brief New information about the chat member
   */
  ChatMember::Ptr newChatMember;

  /**
   * @brief Optional. Chat invite link, which was used by the user to join the chat; for joining by invite
   * link events only.
   */
  ChatInviteLink::Ptr inviteLink;

  /**
   * @brief Optional. True, if the user joined the chat via a chat folder invite link
   */
  bool viaChatFolderInviteLink;
};
}  // namespace tgbm
