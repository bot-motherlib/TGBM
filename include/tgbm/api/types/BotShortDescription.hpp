#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*This object represents the bot's short description.*/
struct BotShortDescription {
  /* The bot's short description */
  String short_description;

  constexpr static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("short_description", true).or_default(false);
  }
};

}  // namespace tgbm::api
