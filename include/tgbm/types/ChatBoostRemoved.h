#pragma once

#include "tgbm/types/Chat.h"
#include "tgbm/types/ChatBoostSource.h"

#include <cstdint>
#include <memory>
#include <string>

namespace tgbm {

/**
 * @brief This object represents a boost removed from a chat.
 *
 * @ingroup types
 */
class ChatBoostRemoved {
 public:
  typedef std::shared_ptr<ChatBoostRemoved> Ptr;

  /**
   * @brief Chat which was boosted
   */
  Chat::Ptr chat;

  /**
   * @brief Unique identifier of the boost
   */
  std::string boostId;

  /**
   * @brief Point in time (Unix timestamp) when the boost was removed
   */
  std::uint32_t removeDate;

  /**
   * @brief Source of the removed boost
   */
  ChatBoostSource::Ptr source;
};
}  // namespace tgbm
