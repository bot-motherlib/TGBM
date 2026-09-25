#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*A bot command.*/
struct RichTextBotCommand {
  /* The text */
  box<RichText> text;
  /* The bot command */
  String bot_command;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("text", true).case_("bot_command", true).or_default(false);
  }
};

}  // namespace tgbm::api
