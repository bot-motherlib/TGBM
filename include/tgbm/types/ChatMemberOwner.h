#pragma once

#include "tgbm/types/ChatMember.h"

#include <memory>
#include <string>

namespace tgbm {

/**
 * @brief Represents a chat member that owns the chat and has all administrator privileges.
 *
 * @ingroup types
 */
class ChatMemberOwner : public ChatMember {
 public:
  static const std::string STATUS;

  typedef std::shared_ptr<ChatMemberOwner> Ptr;

  ChatMemberOwner() {
    this->status = STATUS;
  }

  /**
   * @brief Custom title for this user
   */
  std::string customTitle;

  /**
   * @brief True, if the user's presence in the chat is hidden
   */
  bool isAnonymous;
};
}  // namespace tgbm
