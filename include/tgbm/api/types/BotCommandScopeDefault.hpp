#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*Represents the default scope of bot commands. Default commands are used if no commands with a narrower scope
 * are specified for the user.*/
struct BotCommandScopeDefault {
  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).or_default(false);
  }

  bool operator==(const BotCommandScopeDefault&) const;
  std::strong_ordering operator<=>(const BotCommandScopeDefault&) const;
};

}  // namespace tgbm::api
