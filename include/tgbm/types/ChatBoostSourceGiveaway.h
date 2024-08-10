#ifndef TGBOT_CHATBOOSTSOURCEGIVEAWAY_H
#define TGBOT_CHATBOOSTSOURCEGIVEAWAY_H

#include "tgbm/types/ChatBoostSource.h"

#include <cstdint>
#include <memory>

namespace tgbm {

/**
 * @brief The boost was obtained by the creation of a Telegram Premium giveaway.
 *
 * This boosts the chat 4 times for the duration of the corresponding Telegram Premium subscription.
 *
 * @ingroup types
 */
class ChatBoostSourceGiveaway : public ChatBoostSource {
 public:
  static const std::string SOURCE;

  typedef std::shared_ptr<ChatBoostSourceGiveaway> Ptr;

  ChatBoostSourceGiveaway() {
    this->source = SOURCE;
  }

  /**
   * @brief Identifier of a message in the chat with the giveaway; the message could have been deleted
   * already.
   *
   * May be 0 if the message isn't sent yet.
   */
  std::int32_t giveawayMessageId;

  /**
   * @brief Optional. True, if the giveaway was completed, but there was no user to win the prize
   */
  bool isUnclaimed;
};
}  // namespace tgbm

#endif  // TGBOT_CHATBOOSTSOURCEGIVEAWAY_H
