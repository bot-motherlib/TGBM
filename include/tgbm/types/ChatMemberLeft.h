#ifndef TGBOT_CHATMEMBERLEFT_H
#define TGBOT_CHATMEMBERLEFT_H

#include "tgbm/types/ChatMember.h"

#include <memory>

namespace tgbm {

/**
 * @brief Represents a chat member that isn't currently a member of the chat, but may join it themselves.
 *
 * @ingroup types
 */
class ChatMemberLeft : public ChatMember {
 public:
  static const std::string STATUS;

  typedef std::shared_ptr<ChatMemberLeft> Ptr;

  ChatMemberLeft() {
    this->status = STATUS;
  }
};
}  // namespace tgbm

#endif  // TGBOT_CHATMEMBERLEFT_H
