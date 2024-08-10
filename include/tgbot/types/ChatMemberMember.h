#ifndef TGBOT_CHATMEMBERMEMBER_H
#define TGBOT_CHATMEMBERMEMBER_H

#include "tgbot/types/ChatMember.h"

#include <memory>

namespace tgbm {

/**
 * @brief Represents a chat member that has no additional privileges or restrictions.
 *
 * @ingroup types
 */
class ChatMemberMember : public ChatMember {
 public:
  static const std::string STATUS;

  typedef std::shared_ptr<ChatMemberMember> Ptr;

  ChatMemberMember() {
    this->status = STATUS;
  }
};
}  // namespace tgbm

#endif  // TGBOT_CHATMEMBERMEMBER_H
