#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*Describes an amount of Telegram Stars.*/
struct StarAmount {
  /* Integer amount of Telegram Stars, rounded to 0; can be negative */
  Integer amount;
  /* Optional. The number of 1/1000000000 shares of Telegram Stars; from -999999999 to 999999999; can be
   * negative if and only if amount is non-positive */
  optional<Integer> nanostar_amount;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("amount", true).or_default(false);
  }
};

}  // namespace tgbm::api
