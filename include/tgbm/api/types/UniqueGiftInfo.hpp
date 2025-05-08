#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*Describes a service message about a unique gift that was sent or received.*/
struct UniqueGiftInfo {
  /* Information about the gift */
  box<UniqueGift> gift;
  /* Origin of the gift. Currently, either “upgrade” or “transfer” */
  String origin;
  /* Optional. Unique identifier of the received gift for the bot; only present for gifts received on behalf
   * of business accounts */
  optional<String> owned_gift_id;
  /* Optional. Number of Telegram Stars that must be paid to transfer the gift; omitted if the bot cannot
   * transfer the gift */
  optional<Integer> transfer_star_count;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("gift", true).case_("origin", true).or_default(false);
  }
};

}  // namespace tgbm::api
