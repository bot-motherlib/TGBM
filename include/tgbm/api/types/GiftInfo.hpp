#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*Describes a service message about a regular gift that was sent or received.*/
struct GiftInfo {
  /* Information about the gift */
  box<Gift> gift;
  /* Optional. Unique identifier of the received gift for the bot; only present for gifts received on behalf
   * of business accounts */
  optional<String> owned_gift_id;
  /* Optional. Number of Telegram Stars that can be claimed by the receiver by converting the gift; omitted if
   * conversion to Telegram Stars is impossible */
  optional<Integer> convert_star_count;
  /* Optional. Number of Telegram Stars that were prepaid for the ability to upgrade the gift */
  optional<Integer> prepaid_upgrade_star_count;
  /* Optional. Text of the message that was added to the gift */
  optional<String> text;
  /* Optional. Special entities that appear in the text */
  optional<arrayof<MessageEntity>> entities;
  /* Optional. Unique number reserved for this gift when upgraded. See the number field in UniqueGift */
  optional<Integer> unique_gift_number;
  /* Optional. True, if the gift's upgrade was purchased after the gift was sent */
  optional<True> is_upgrade_separate;
  /* Optional. True, if the gift can be upgraded to a unique gift */
  optional<True> can_be_upgraded;
  /* Optional. True, if the sender and gift text are shown only to the gift receiver; otherwise, everyone will
   * be able to see them */
  optional<True> is_private;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("gift", true).or_default(false);
  }
};

}  // namespace tgbm::api
