#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*This object represents the bot's name.*/
struct BotName {
  /* The bot's name */
  String name;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("name", true).or_default(false);
  }

  bool operator==(const BotName&) const;
  std::strong_ordering operator<=>(const BotName&) const;
};

}  // namespace tgbm::api
