#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*This object represents a service message about a user boosting a chat.*/
struct ChatBoostAdded {
  /* Number of boosts added by the user */
  Integer boost_count;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("boost_count", true).or_default(false);
  }
};

}  // namespace tgbm::api
