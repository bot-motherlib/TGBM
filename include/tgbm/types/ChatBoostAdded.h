#pragma once

#include <cstdint>
#include <memory>

namespace tgbm {

/**
 * @brief This object represents a service message about a user boosting a chat.
 *
 * @ingroup types
 */
class ChatBoostAdded {
 public:
  typedef std::shared_ptr<ChatBoostAdded> Ptr;

  /**
   * @brief Number of boosts added by the user
   */
  std::int32_t boostCount;
};
}  // namespace tgbm
