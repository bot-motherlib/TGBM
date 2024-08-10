#pragma once

#include <cstdint>
#include <memory>

namespace tgbm {

/**
 * @brief This object represents a service message about a change in auto-delete timer settings.
 *
 * @ingroup types
 */
class MessageAutoDeleteTimerChanged {
 public:
  typedef std::shared_ptr<MessageAutoDeleteTimerChanged> Ptr;

  /**
   * @brief New auto-delete time for messages in the chat
   */
  std::int32_t messageAutoDeleteTime;
};
}  // namespace tgbm
