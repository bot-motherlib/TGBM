#ifndef TGBOT_GIVEAWAYCOMPLETED_H
#define TGBOT_GIVEAWAYCOMPLETED_H

#include <cstdint>
#include <memory>

namespace tgbm {

class Message;

/**
 * @brief This object represents a service message about the completion of a giveaway without public winners.
 *
 * @ingroup types
 */
class GiveawayCompleted {
 public:
  typedef std::shared_ptr<GiveawayCompleted> Ptr;

  /**
   * @brief Number of winners in the giveaway
   */
  std::int32_t winnerCount;

  /**
   * @brief Optional. Number of undistributed prizes
   */
  std::int32_t unclaimedPrizeCount;

  /**
   * @brief Optional. Message with the giveaway that was completed, if it wasn't deleted
   */
  std::shared_ptr<Message> giveawayMessage;
};
}  // namespace tgbm

#endif  // TGBOT_GIVEAWAYCOMPLETED_H