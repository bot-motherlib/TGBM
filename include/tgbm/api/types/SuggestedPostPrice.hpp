#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*Describes the price of a suggested post.*/
struct SuggestedPostPrice {
  /* Currency in which the post will be paid. Currently, must be one of “XTR” for Telegram Stars or “TON” for
   * toncoins */
  String currency;
  /* The amount of the currency that will be paid for the post in the smallest units of the currency, i.e.
   * Telegram Stars or nanotoncoins. Currently, price in Telegram Stars must be between 5 and 100000, and
   * price in nanotoncoins must be between 10000000 and 10000000000000. */
  Integer amount;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("currency", true).case_("amount", true).or_default(false);
  }
};

}  // namespace tgbm::api
