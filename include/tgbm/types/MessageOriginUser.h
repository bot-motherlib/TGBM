#pragma once

#include "tgbm/types/MessageOrigin.h"
#include "tgbm/types/User.h"

#include <memory>

namespace tgbm {

/**
 * @brief The message was originally sent by a known user.
 *
 * @ingroup types
 */
class MessageOriginUser : public MessageOrigin {
 public:
  static const std::string TYPE;

  typedef std::shared_ptr<MessageOriginUser> Ptr;

  MessageOriginUser() {
    this->type = TYPE;
  }

  /**
   * @brief User that sent the message originally
   */
  User::Ptr senderUser;
};
}  // namespace tgbm
