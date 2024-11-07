#pragma once

#include "all_fwd.hpp"

namespace tgbm::api {

/*This object describes the type of a reaction. Currently, it can be one of*/
struct ReactionTypePaid {
  consteval static bool is_mandatory_field(std::string_view name) {
    return utils::string_switch<bool>(name).or_default(false);
  }
};

}  // namespace tgbm::api
