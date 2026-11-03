#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*Represents a menu button, which opens the bot's list of commands.*/
struct MenuButtonCommands {
  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).or_default(false);
  }

  bool operator==(const MenuButtonCommands&) const;
  std::strong_ordering operator<=>(const MenuButtonCommands&) const;
};

}  // namespace tgbm::api
