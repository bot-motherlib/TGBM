#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*This object describes a gift received and owned by a user or a chat. Currently, it can be one of*/
struct OwnedGiftRegular {
  /* Information about the regular gift */
  box<Gift> gift;
  /* Date the gift was sent in Unix time */
  Integer send_date;
  /* Optional. Unique identifier of the gift for the bot; for gifts received on behalf of business accounts
   * only */
  optional<String> owned_gift_id;
  /* Optional. Sender of the gift if it is a known user */
  box<User> sender_user;
  /* Optional. Text of the message that was added to the gift */
  optional<String> text;
  /* Optional. Special entities that appear in the text */
  optional<arrayof<MessageEntity>> entities;
  /* Optional. Number of Telegram Stars that can be claimed by the receiver instead of the gift; omitted if
   * the gift cannot be converted to Telegram Stars */
  optional<Integer> convert_star_count;
  /* Optional. Number of Telegram Stars that were paid by the sender for the ability to upgrade the gift */
  optional<Integer> prepaid_upgrade_star_count;
  /* Optional. True, if the sender and gift text are shown only to the gift receiver; otherwise, everyone will
   * be able to see them */
  optional<True> is_private;
  /* Optional. True, if the gift is displayed on the account's profile page; for gifts received on behalf of
   * business accounts only */
  optional<True> is_saved;
  /* Optional. True, if the gift can be upgraded to a unique gift; for gifts received on behalf of business
   * accounts only */
  optional<True> can_be_upgraded;
  /* Optional. True, if the gift was refunded and isn't available anymore */
  optional<True> was_refunded;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("gift", true).case_("send_date", true).or_default(false);
  }
};

}  // namespace tgbm::api
