#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*This object contains information about changes to a user payment subscription toward the current bot.*/
struct BotSubscriptionUpdated {
  /* User who subscribed for payments toward the bot */
  box<User> user;
  /* Bot-specified invoice payload */
  String invoice_payload;
  /* The new state of the subscription. Currently, it can be one of “canceled” if the user canceled the
   * subscription, “active” if the user re-enabled a previously canceled subscription, or “failed” if payment
   * for the subscription failed. */
  String state;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name)
        .case_("user", true)
        .case_("invoice_payload", true)
        .case_("state", true)
        .or_default(false);
  }
};

}  // namespace tgbm::api
