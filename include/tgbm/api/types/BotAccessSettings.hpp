#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*This object describes the access settings of a bot.*/
struct BotAccessSettings {
  /* True, if only selected users can access the bot. The bot's owner can always access it. */
  bool is_access_restricted;
  /* Optional. The list of other users who have access to the bot if the access is restricted */
  optional<arrayof<User>> added_users;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("is_access_restricted", true).or_default(false);
  }
};

}  // namespace tgbm::api
