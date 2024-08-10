#pragma once

#include <cstdint>
#include <memory>
#include <string>

namespace tgbm {

/**
 * @brief This object describes the origin of a message.
 *
 * It can be one of
 * - MessageOriginUser
 * - MessageOriginHiddenUser
 * - MessageOriginChat
 * - MessageOriginChannel
 *
 * @ingroup types
 */
class MessageOrigin {
 public:
  typedef std::shared_ptr<MessageOrigin> Ptr;

  MessageOrigin() {
  }

  virtual ~MessageOrigin() {
  }

  /**
   * @brief Type of the message origin
   */
  std::string type;

  /**
   * @brief Date the message was sent originally in Unix time
   */
  std::uint32_t date;
};
}  // namespace tgbm
