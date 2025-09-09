#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*Describes a service message about a change in the price of direct messages sent to a channel chat.*/
struct DirectMessagePriceChanged {
  /* True, if direct messages are enabled for the channel chat; false otherwise */
  bool are_direct_messages_enabled;
  /* Optional. The new number of Telegram Stars that must be paid by users for each direct message sent to the
   * channel. Does not apply to users who have been exempted by administrators. Defaults to 0. */
  optional<Integer> direct_message_star_count;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("are_direct_messages_enabled", true).or_default(false);
  }
};

}  // namespace tgbm::api
