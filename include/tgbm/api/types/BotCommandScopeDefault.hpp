#pragma once

#include "all_fwd.hpp"

namespace tgbm::api {

/*This object represents the scope to which bot commands are applied. Currently, the following 7 scopes are
 * supported:*/
struct BotCommandScopeDefault {
  consteval static bool is_mandatory_field(std::string_view name) {
    return utils::string_switch<bool>(name).or_default(false);
  }
};

}  // namespace tgbm::api
