#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*This object represents the scope to which bot commands are applied. Currently, the following 7 scopes are
 * supported:*/
struct BotCommandScopeChatMember {
  /* Unique identifier for the target chat or username of the target supergroup (in the format
   * @supergroupusername). Channel direct messages chats and channel chats aren't supported. */
  int_or_str chat_id;
  /* Unique identifier of the target user */
  Integer user_id;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("chat_id", true).case_("user_id", true).or_default(false);
  }
};

}  // namespace tgbm::api
