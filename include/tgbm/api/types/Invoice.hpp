#pragma once

#include "all_fwd.hpp"

namespace tgbm::api {

/*This object contains basic information about an invoice.*/
struct Invoice {
  /* Product name */
  String title;
  /* Product description */
  String description;
  /* Unique bot deep-linking parameter that can be used to generate this invoice */
  String start_parameter;
  /* Three-letter ISO 4217 currency code, or “XTR” for payments in Telegram Stars */
  String currency;
  /* Total price in the smallest units of the currency (integer, not float/double). For example, for a price
   * of US$ 1.45 pass amount = 145. See the exp parameter in currencies.json, it shows the number of digits
   * past the decimal point for each currency (2 for the majority of currencies). */
  Integer total_amount;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name)
        .case_("title", true)
        .case_("description", true)
        .case_("start_parameter", true)
        .case_("currency", true)
        .case_("total_amount", true)
        .or_default(false);
  }
};

}  // namespace tgbm::api
