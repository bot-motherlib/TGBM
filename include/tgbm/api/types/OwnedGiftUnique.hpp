#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*This object describes a gift received and owned by a user or a chat. Currently, it can be one of*/
struct OwnedGiftUnique {
  /* Information about the unique gift */
  box<UniqueGift> gift;
  /* Date the gift was sent in Unix time */
  Integer send_date;
  /* Optional. Unique identifier of the received gift for the bot; for gifts received on behalf of business
   * accounts only */
  optional<String> owned_gift_id;
  /* Optional. Sender of the gift if it is a known user */
  box<User> sender_user;
  /* Optional. Number of Telegram Stars that must be paid to transfer the gift; omitted if the bot cannot
   * transfer the gift */
  optional<Integer> transfer_star_count;
  /* Optional. True, if the gift is displayed on the account's profile page; for gifts received on behalf of
   * business accounts only */
  optional<True> is_saved;
  /* Optional. True, if the gift can be transferred to another owner; for gifts received on behalf of business
   * accounts only */
  optional<True> can_be_transferred;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("gift", true).case_("send_date", true).or_default(false);
  }
};

}  // namespace tgbm::api
