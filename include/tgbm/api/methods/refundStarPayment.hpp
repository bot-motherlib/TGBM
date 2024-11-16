#pragma once

#include <tgbm/api/common.hpp>

namespace tgbm::api {

struct refund_star_payment_request {
  /* Identifier of the user whose payment will be refunded */
  Integer user_id;
  /* Telegram payment identifier */
  String telegram_payment_charge_id;

  using return_type = bool;
  static constexpr file_info_e file_info = file_info_e::no;
  static constexpr std::string_view api_method_name = "refundStarPayment";
  static constexpr http_method_e http_method = http_method_e::POST;

  void fill_nonfile_args(auto& body) const {
    body.arg("user_id", user_id);
    body.arg("telegram_payment_charge_id", telegram_payment_charge_id);
  }
};

}  // namespace tgbm::api
