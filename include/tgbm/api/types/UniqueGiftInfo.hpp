#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*Describes a service message about a unique gift that was sent or received.*/
struct UniqueGiftInfo {
  /* Information about the gift */
  box<UniqueGift> gift;
  /* Origin of the gift. Currently, either “upgrade” for gifts upgraded from regular gifts, “transfer” for
   * gifts transferred from other users or channels, or “resale” for gifts bought from other users */
  String origin;
  /* Optional. For gifts bought from other users, the price paid for the gift */
  optional<Integer> last_resale_star_count;
  /* Optional. Unique identifier of the received gift for the bot; only present for gifts received on behalf
   * of business accounts */
  optional<String> owned_gift_id;
  /* Optional. Number of Telegram Stars that must be paid to transfer the gift; omitted if the bot cannot
   * transfer the gift */
  optional<Integer> transfer_star_count;
  /* Optional. Point in time (Unix timestamp) when the gift can be transferred. If it is in the past, then the
   * gift can be transferred now */
  optional<Integer> next_transfer_date;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("gift", true).case_("origin", true).or_default(false);
  }
};

}  // namespace tgbm::api
