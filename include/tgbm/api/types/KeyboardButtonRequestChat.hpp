#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*This object defines the criteria used to request a suitable chat. Information about the selected chat will
 * be shared with the bot when the corresponding button is pressed. The bot will be granted requested rights
 * in the chat if appropriate. More about requesting chats ».*/
struct KeyboardButtonRequestChat {
  /* Signed 32-bit identifier of the request, which will be received back in the ChatShared object. Must be
   * unique within the message */
  Integer request_id;
  /* Pass True to request a channel chat, pass False to request a group or a supergroup chat. */
  bool chat_is_channel;
  /* Optional. A JSON-serialized object listing the required administrator rights of the user in the chat. The
   * rights must be a superset of bot_administrator_rights. If not specified, no additional restrictions are
   * applied. */
  box<ChatAdministratorRights> user_administrator_rights;
  /* Optional. A JSON-serialized object listing the required administrator rights of the bot in the chat. The
   * rights must be a subset of user_administrator_rights. If not specified, no additional restrictions are
   * applied. */
  box<ChatAdministratorRights> bot_administrator_rights;
  /* Optional. Pass True to request a forum supergroup, pass False to request a non-forum chat. If not
   * specified, no additional restrictions are applied. */
  optional<bool> chat_is_forum;
  /* Optional. Pass True to request a supergroup or a channel with a username, pass False to request a chat
   * without a username. If not specified, no additional restrictions are applied. */
  optional<bool> chat_has_username;
  /* Optional. Pass True to request a chat owned by the user. Otherwise, no additional restrictions are
   * applied. */
  optional<bool> chat_is_created;
  /* Optional. Pass True to request a chat with the bot as a member. Otherwise, no additional restrictions are
   * applied. */
  optional<bool> bot_is_member;
  /* Optional. Pass True to request the chat's title */
  optional<bool> request_title;
  /* Optional. Pass True to request the chat's username */
  optional<bool> request_username;
  /* Optional. Pass True to request the chat's photo */
  optional<bool> request_photo;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name)
        .case_("request_id", true)
        .case_("chat_is_channel", true)
        .or_default(false);
  }
};

}  // namespace tgbm::api
