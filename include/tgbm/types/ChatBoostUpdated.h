#pragma once

#include "tgbm/types/Chat.h"
#include "tgbm/types/ChatBoost.h"

#include <memory>

namespace tgbm {

/**
 * @brief This object represents a boost added to a chat or changed.
 *
 * @ingroup types
 */
class ChatBoostUpdated {
 public:
  typedef std::shared_ptr<ChatBoostUpdated> Ptr;

  /**
   * @brief Chat which was boosted
   */
  Chat::Ptr chat;

  /**
   * @brief Information about the chat boost
   */
  ChatBoost::Ptr boost;
};
}  // namespace tgbm
