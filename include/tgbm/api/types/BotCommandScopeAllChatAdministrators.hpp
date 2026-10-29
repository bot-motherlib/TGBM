#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*Represents the scope of bot commands, covering all group and supergroup chat administrators.*/
struct BotCommandScopeAllChatAdministrators {
  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).or_default(false);
  }

  bool operator==(const BotCommandScopeAllChatAdministrators&) const;
  std::strong_ordering operator<=>(const BotCommandScopeAllChatAdministrators&) const;
};

}  // namespace tgbm::api
