#pragma once

#include "all_fwd.hpp"

namespace tgbm::api {

/*This object represents a bot command.*/
struct BotCommand {
  /* Text of the command; 1-32 characters. Can contain only lowercase English letters, digits and underscores.
   */
  String command;
  /* Description of the command; 1-256 characters. */
  String description;

  consteval static bool is_mandatory_field(std::string_view name) {
    return utils::string_switch<bool>(name)
        .case_("command", true)
        .case_("description", true)
        .or_default(false);
  }
};

}  // namespace tgbm::api
