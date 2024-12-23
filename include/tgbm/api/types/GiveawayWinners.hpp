#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*This object represents a message about the completion of a giveaway with public winners.*/
struct GiveawayWinners {
  /* The chat that created the giveaway */
  box<Chat> chat;
  /* Identifier of the message with the giveaway in the chat */
  Integer giveaway_message_id;
  /* Point in time (Unix timestamp) when winners of the giveaway were selected */
  Integer winners_selection_date;
  /* Total number of winners in the giveaway */
  Integer winner_count;
  /* List of up to 100 winners of the giveaway */
  arrayof<User> winners;
  /* Optional. The number of other chats the user had to join in order to be eligible for the giveaway */
  optional<Integer> additional_chat_count;
  /* Optional. The number of Telegram Stars that were split between giveaway winners; for Telegram Star
   * giveaways only */
  optional<Integer> prize_star_count;
  /* Optional. The number of months the Telegram Premium subscription won from the giveaway will be active
   * for; for Telegram Premium giveaways only */
  optional<Integer> premium_subscription_month_count;
  /* Optional. Number of undistributed prizes */
  optional<Integer> unclaimed_prize_count;
  /* Optional. Description of additional giveaway prize */
  optional<String> prize_description;
  /* Optional. True, if only users who had joined the chats after the giveaway started were eligible to win */
  optional<True> only_new_members;
  /* Optional. True, if the giveaway was canceled because the payment for it was refunded */
  optional<True> was_refunded;

  constexpr static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name)
        .case_("chat", true)
        .case_("giveaway_message_id", true)
        .case_("winners_selection_date", true)
        .case_("winner_count", true)
        .case_("winners", true)
        .or_default(false);
  }
};

}  // namespace tgbm::api
