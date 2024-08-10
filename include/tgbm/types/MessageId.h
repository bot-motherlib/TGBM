#pragma once

#include <memory>

namespace tgbm {

/**
 * @brief This object represents a unique message identifier.
 *
 * @ingroup types
 */
class MessageId {
 public:
  typedef std::shared_ptr<MessageId> Ptr;

  /**
   * @brief Unique message identifier
   */
  std::int32_t messageId;
};
}  // namespace tgbm
