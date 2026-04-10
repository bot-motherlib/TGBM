#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*This object contains information about the bot that was created to be managed by the current bot.*/
struct ManagedBotCreated {
  /* Information about the bot. The bot's token can be fetched using the method getManagedBotToken. */
  box<User> bot;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("bot", true).or_default(false);
  }
};

}  // namespace tgbm::api
