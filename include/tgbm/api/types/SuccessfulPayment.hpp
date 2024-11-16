#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*This object contains basic information about a successful payment.*/
struct SuccessfulPayment {
  /* Three-letter ISO 4217 currency code, or “XTR” for payments in Telegram Stars */
  String currency;
  /* Total price in the smallest units of the currency (integer, not float/double). For example, for a price
   * of US$ 1.45 pass amount = 145. See the exp parameter in currencies.json, it shows the number of digits
   * past the decimal point for each currency (2 for the majority of currencies). */
  Integer total_amount;
  /* Bot-specified invoice payload */
  String invoice_payload;
  /* Telegram payment identifier */
  String telegram_payment_charge_id;
  /* Provider payment identifier */
  String provider_payment_charge_id;
  /* Optional. Identifier of the shipping option chosen by the user */
  optional<String> shipping_option_id;
  /* Optional. Order information provided by the user */
  box<OrderInfo> order_info;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name)
        .case_("currency", true)
        .case_("total_amount", true)
        .case_("invoice_payload", true)
        .case_("telegram_payment_charge_id", true)
        .case_("provider_payment_charge_id", true)
        .or_default(false);
  }
};

}  // namespace tgbm::api
