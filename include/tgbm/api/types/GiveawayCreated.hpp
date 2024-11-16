#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*This object represents a service message about the creation of a scheduled giveaway.*/
struct GiveawayCreated {
  /* Optional. The number of Telegram Stars to be split between giveaway winners; for Telegram Star giveaways
   * only */
  optional<Integer> prize_star_count;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).or_default(false);
  }
};

}  // namespace tgbm::api
