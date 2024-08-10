#pragma once

#include "tgbm/types/MessageOrigin.h"

#include <memory>
#include <string>

namespace tgbm {

/**
 * @brief The message was originally sent by an unknown user.
 *
 * @ingroup types
 */
class MessageOriginHiddenUser : public MessageOrigin {
 public:
  static const std::string TYPE;

  typedef std::shared_ptr<MessageOriginHiddenUser> Ptr;

  MessageOriginHiddenUser() {
    this->type = TYPE;
  }

  /**
   * @brief Name of the user that sent the message originally
   */
  std::string senderUserName;
};
}  // namespace tgbm
