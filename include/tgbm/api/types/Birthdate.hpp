#pragma once

#include "all_fwd.hpp"

namespace tgbm::api {

/*Describes the birthdate of a user.*/
struct Birthdate {
  /* Day of the user's birth; 1-31 */
  Integer day;
  /* Month of the user's birth; 1-12 */
  Integer month;
  /* Optional. Year of the user's birth */
  optional<Integer> year;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("day", true).case_("month", true).or_default(false);
  }
};

}  // namespace tgbm::api
