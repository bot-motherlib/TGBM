#pragma once

#include "tgbm/types/Location.h"

#include <memory>

namespace tgbm {

/**
 * @brief Represents a location to which a chat is connected.
 *
 * @ingroup types
 */
class ChatLocation {
 public:
  typedef std::shared_ptr<ChatLocation> Ptr;

  /**
   * @brief The location to which the supergroup is connected.
   * Can't be a live location.
   */
  Location::Ptr location;

  /**
   * @brief Location address; 1-64 characters, as defined by the chat owner
   */
  std::string address;
};
}  // namespace tgbm
