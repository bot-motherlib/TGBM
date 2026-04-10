#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*This object contains information about the creation, token update, or owner update of a bot that is managed
 * by the current bot.*/
struct ManagedBotUpdated {
  /* User that created the bot */
  box<User> user;
  /* Information about the bot. Token of the bot can be fetched using the method getManagedBotToken. */
  box<User> bot;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("user", true).case_("bot", true).or_default(false);
  }
};

}  // namespace tgbm::api
