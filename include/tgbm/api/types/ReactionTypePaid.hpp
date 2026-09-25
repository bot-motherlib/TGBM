#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*The reaction is paid.*/
struct ReactionTypePaid {
  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).or_default(false);
  }
};

}  // namespace tgbm::api
