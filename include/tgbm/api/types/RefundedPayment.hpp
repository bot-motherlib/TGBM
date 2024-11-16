#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*This object contains basic information about a refunded payment.*/
struct RefundedPayment {
  /* Three-letter ISO 4217 currency code, or “XTR” for payments in Telegram Stars. Currently, always “XTR” */
  String currency;
  /* Total refunded price in the smallest units of the currency (integer, not float/double). For example, for
   * a price of US$ 1.45, total_amount = 145. See the exp parameter in currencies.json, it shows the number of
   * digits past the decimal point for each currency (2 for the majority of currencies). */
  Integer total_amount;
  /* Bot-specified invoice payload */
  String invoice_payload;
  /* Telegram payment identifier */
  String telegram_payment_charge_id;
  /* Optional. Provider payment identifier */
  optional<String> provider_payment_charge_id;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name)
        .case_("currency", true)
        .case_("total_amount", true)
        .case_("invoice_payload", true)
        .case_("telegram_payment_charge_id", true)
        .or_default(false);
  }
};

}  // namespace tgbm::api
