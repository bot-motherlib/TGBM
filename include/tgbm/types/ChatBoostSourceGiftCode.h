#pragma once

#include "tgbm/types/ChatBoostSource.h"

#include <memory>

namespace tgbm {

/**
 * @brief The boost was obtained by the creation of Telegram Premium gift codes to boost a chat.
 *
 * Each such code boosts the chat 4 times for the duration of the corresponding Telegram Premium subscription.
 *
 * @ingroup types
 */
class ChatBoostSourceGiftCode : public ChatBoostSource {
 public:
  static const std::string SOURCE;

  typedef std::shared_ptr<ChatBoostSourceGiftCode> Ptr;

  ChatBoostSourceGiftCode() {
    this->source = SOURCE;
  }
};
}  // namespace tgbm
