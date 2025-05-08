#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*Describes a service message about a change in the price of paid messages within a chat.*/
struct PaidMessagePriceChanged {
  /* The new number of Telegram Stars that must be paid by non-administrator users of the supergroup chat for
   * each sent message */
  Integer paid_message_star_count;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("paid_message_star_count", true).or_default(false);
  }
};

}  // namespace tgbm::api
