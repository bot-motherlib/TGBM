#pragma once

#include "all_fwd.hpp"

namespace tgbm::api {

/*This object represents a message about a scheduled giveaway.*/
struct Giveaway {
  /* The list of chats which the user must join to participate in the giveaway */
  arrayof<Chat> chats;
  /* Point in time (Unix timestamp) when winners of the giveaway will be selected */
  Integer winners_selection_date;
  /* The number of users which are supposed to be selected as winners of the giveaway */
  Integer winner_count;
  /* Optional. Description of additional giveaway prize */
  optional<String> prize_description;
  /* Optional. A list of two-letter ISO 3166-1 alpha-2 country codes indicating the countries from which
   * eligible users for the giveaway must come. If empty, then all users can participate in the giveaway.
   * Users with a phone number that was bought on Fragment can always participate in giveaways. */
  optional<arrayof<String>> country_codes;
  /* Optional. The number of Telegram Stars to be split between giveaway winners; for Telegram Star giveaways
   * only */
  optional<Integer> prize_star_count;
  /* Optional. The number of months the Telegram Premium subscription won from the giveaway will be active
   * for; for Telegram Premium giveaways only */
  optional<Integer> premium_subscription_month_count;
  /* Optional. True, if only users who join the chats after the giveaway started should be eligible to win */
  optional<True> only_new_members;
  /* Optional. True, if the list of giveaway winners will be visible to everyone */
  optional<True> has_public_winners;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name)
        .case_("chats", true)
        .case_("winners_selection_date", true)
        .case_("winner_count", true)
        .or_default(false);
  }
};

}  // namespace tgbm::api
