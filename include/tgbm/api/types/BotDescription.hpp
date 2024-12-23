#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*This object represents the bot's description.*/
struct BotDescription {
  /* The bot's description */
  String description;

  constexpr static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("description", true).or_default(false);
  }
};

}  // namespace tgbm::api
