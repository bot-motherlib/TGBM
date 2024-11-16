#pragma once

#include "all_fwd.hpp"

namespace tgbm::api {

/*This object represents a portion of the price for goods or services.*/
struct LabeledPrice {
  /* Portion label */
  String label;
  /* Price of the product in the smallest units of the currency (integer, not float/double). For example, for
   * a price of US$ 1.45 pass amount = 145. See the exp parameter in currencies.json, it shows the number of
   * digits past the decimal point for each currency (2 for the majority of currencies). */
  Integer amount;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("label", true).case_("amount", true).or_default(false);
  }
};

}  // namespace tgbm::api
