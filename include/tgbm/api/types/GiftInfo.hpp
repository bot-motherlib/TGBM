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
  /* Optional. Number of Telegram Stars that were prepaid by the sender for the ability to upgrade the gift */
  optional<Integer> prepaid_upgrade_star_count;
  /* Optional. Text of the message that was added to the gift */
  optional<String> text;
  /* Optional. Special entities that appear in the text */
  optional<arrayof<MessageEntity>> entities;
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
