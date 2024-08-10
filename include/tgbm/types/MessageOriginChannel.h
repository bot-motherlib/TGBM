#pragma once

#include "tgbm/types/MessageOrigin.h"
#include "tgbm/types/Chat.h"

#include <cstdint>
#include <memory>
#include <string>

namespace tgbm {

/**
 * @brief The message was originally sent to a channel chat.
 *
 * @ingroup types
 */
class MessageOriginChannel : public MessageOrigin {
 public:
  static const std::string TYPE;

  typedef std::shared_ptr<MessageOriginChannel> Ptr;

  MessageOriginChannel() {
    this->type = TYPE;
  }

  /**
   * @brief Channel chat to which the message was originally sent
   */
  Chat::Ptr chat;

  /**
   * @brief Unique message identifier inside the chat
   */
  std::int32_t messageId;

  /**
   * @brief Optional. Signature of the original post author
   */
  std::string authorSignature;
};
}  // namespace tgbm
