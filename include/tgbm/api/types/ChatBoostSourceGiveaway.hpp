#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*The boost was obtained by the creation of a Telegram Premium or a Telegram Star giveaway. This boosts the
 * chat 4 times for the duration of the corresponding Telegram Premium subscription for Telegram Premium
 * giveaways and prize_star_count / 500 times for one year for Telegram Star giveaways.*/
struct ChatBoostSourceGiveaway {
  /* Identifier of a message in the chat with the giveaway; the message could have been deleted already. May
   * be 0 if the message isn't sent yet. */
  Integer giveaway_message_id;
  /* Optional. User that won the prize in the giveaway if any; for Telegram Premium giveaways only */
  box<User> user;
  /* Optional. The number of Telegram Stars to be split between giveaway winners; for Telegram Star giveaways
   * only */
  optional<Integer> prize_star_count;
  /* Optional. True, if the giveaway was completed, but there was no user to win the prize */
  optional<True> is_unclaimed;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("giveaway_message_id", true).or_default(false);
  }
};

}  // namespace tgbm::api
