#pragma once

#include <tgbm/api/common.hpp>

namespace tgbm::api {

struct edit_user_star_subscription_request {
  /* Identifier of the user whose subscription will be edited */
  Integer user_id;
  /* Telegram payment identifier for the subscription */
  String telegram_payment_charge_id;
  /* Pass True to cancel extension of the user subscription; the subscription must be active up to the end of
   * the current subscription period. Pass False to allow the user to re-enable a subscription that was
   * previously canceled by the bot. */
  bool is_canceled;

  using return_type = bool;
  static constexpr file_info_e file_info = file_info_e::no;
  static constexpr std::string_view api_method_name = "editUserStarSubscription";
  static constexpr http_method_e http_method = http_method_e::POST;

  void fill_nonfile_args(auto& body) const {
    body.arg("user_id", user_id);
    body.arg("telegram_payment_charge_id", telegram_payment_charge_id);
    body.arg("is_canceled", is_canceled);
  }
};

}  // namespace tgbm::api
