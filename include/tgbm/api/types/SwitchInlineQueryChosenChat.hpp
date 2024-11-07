#pragma once

#include "all_fwd.hpp"

namespace tgbm::api {

/*This object represents an inline button that switches the current user to inline mode in a chosen chat, with
 * an optional default inline query.*/
struct SwitchInlineQueryChosenChat {
  /* Optional. The default inline query to be inserted in the input field. If left empty, only the bot's
   * username will be inserted */
  optional<String> query;
  /* Optional. True, if private chats with users can be chosen */
  optional<bool> allow_user_chats;
  /* Optional. True, if private chats with bots can be chosen */
  optional<bool> allow_bot_chats;
  /* Optional. True, if group and supergroup chats can be chosen */
  optional<bool> allow_group_chats;
  /* Optional. True, if channel chats can be chosen */
  optional<bool> allow_channel_chats;

  consteval static bool is_mandatory_field(std::string_view name) {
    return utils::string_switch<bool>(name).or_default(false);
  }
};

}  // namespace tgbm::api
