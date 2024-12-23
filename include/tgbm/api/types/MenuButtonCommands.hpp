#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*This object describes the bot's menu button in a private chat. It should be one of*/
struct MenuButtonCommands {
  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).or_default(false);
  }
};

}  // namespace tgbm::api
