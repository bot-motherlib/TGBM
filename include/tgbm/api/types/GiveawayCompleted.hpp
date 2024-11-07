#pragma once

#include "all_fwd.hpp"

namespace tgbm::api {

/*This object represents a service message about the completion of a giveaway without public winners.*/
struct GiveawayCompleted {
  /* Number of winners in the giveaway */
  Integer winner_count;
  /* Optional. Number of undistributed prizes */
  optional<Integer> unclaimed_prize_count;
  /* Optional. Message with the giveaway that was completed, if it wasn't deleted */
  box<Message> giveaway_message;
  /* Optional. True, if the giveaway is a Telegram Star giveaway. Otherwise, currently, the giveaway is a
   * Telegram Premium giveaway. */
  optional<True> is_star_giveaway;

  consteval static bool is_mandatory_field(std::string_view name) {
    return utils::string_switch<bool>(name).case_("winner_count", true).or_default(false);
  }
};

}  // namespace tgbm::api
