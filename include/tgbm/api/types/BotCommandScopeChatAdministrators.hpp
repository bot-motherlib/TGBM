#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*This object represents the scope to which bot commands are applied. Currently, the following 7 scopes are
 * supported:*/
struct BotCommandScopeChatAdministrators {
  /* Unique identifier for the target chat or username of the target supergroup (in the format
   * @supergroupusername) */
  int_or_str chat_id;

  constexpr static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("chat_id", true).or_default(false);
  }
};

}  // namespace tgbm::api
