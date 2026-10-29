#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*Represents the scope of bot commands, covering all private chats.*/
struct BotCommandScopeAllPrivateChats {
  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).or_default(false);
  }

  bool operator==(const BotCommandScopeAllPrivateChats&) const;
  std::strong_ordering operator<=>(const BotCommandScopeAllPrivateChats&) const;
};

}  // namespace tgbm::api
