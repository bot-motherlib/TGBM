#pragma once

#include "all_fwd.hpp"

namespace tgbm::api {

/*This object represents the bot's name.*/
struct BotName {
  /* The bot's name */
  String name;

  consteval static bool is_mandatory_field(std::string_view name) {
    return utils::string_switch<bool>(name).case_("name", true).or_default(false);
  }
};

}  // namespace tgbm::api
