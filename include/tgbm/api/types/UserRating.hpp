#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*This object describes the rating of a user based on their Telegram Star spendings.*/
struct UserRating {
  /* Current level of the user, indicating their reliability when purchasing digital goods and services. A
   * higher level suggests a more trustworthy customer; a negative level is likely reason for concern. */
  Integer level;
  /* Numerical value of the user's rating; the higher the rating, the better */
  Integer rating;
  /* The rating value required to get the current level */
  Integer current_level_rating;
  /* Optional. The rating value required to get to the next level; omitted if the maximum level was reached */
  optional<Integer> next_level_rating;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name)
        .case_("level", true)
        .case_("rating", true)
        .case_("current_level_rating", true)
        .or_default(false);
  }
};

}  // namespace tgbm::api
