#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*This object describes the source of a chat boost. It can be one of*/
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
