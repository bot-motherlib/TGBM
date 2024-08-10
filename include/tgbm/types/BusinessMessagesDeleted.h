#pragma once

#include "tgbm/types/Chat.h"

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace tgbm {

/**
 * @brief This object is received when messages are deleted from a connected business account.
 *
 * @ingroup types
 */
class BusinessMessagesDeleted {
 public:
  typedef std::shared_ptr<BusinessMessagesDeleted> Ptr;

  /**
   * @brief Unique identifier of the business connection
   */
  std::string businessConnectionId;

  /**
   * @brief Information about a chat in the business account.
   *
   * The bot may not have access to the chat or the corresponding user.
   */
  Chat::Ptr chat;

  /**
   * @brief A JSON-serialized list of identifiers of deleted messages in the chat of the business account
   */
  std::vector<std::int32_t> messageIds;
};
}  // namespace tgbm
