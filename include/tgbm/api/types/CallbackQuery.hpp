#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*This object represents an incoming callback query from a callback button in an inline keyboard. If the
 * button that originated the query was attached to a message sent by the bot, the field message will be
 * present. If the button was attached to a message sent via the bot (in inline mode), the field
 * inline_message_id will be present. Exactly one of the fields data or game_short_name will be present.*/
struct CallbackQuery {
  /* Unique identifier for this query */
  String id;
  /* Sender */
  box<User> from;
  /* Global identifier, uniquely corresponding to the chat to which the message with the callback button was
   * sent. Useful for high scores in games. */
  String chat_instance;
  /* Optional. Message sent by the bot with the callback button that originated the query */
  box<MaybeInaccessibleMessage> message;
  /* Optional. Identifier of the message sent via the bot in inline mode, that originated the query. */
  optional<String> inline_message_id;
  /* Optional. Data associated with the callback button. Be aware that the message originated the query can
   * contain no callback buttons with this data. */
  optional<String> data;
  /* Optional. Short name of a Game to be returned, serves as the unique identifier for the game */
  optional<String> game_short_name;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name)
        .case_("id", true)
        .case_("from", true)
        .case_("chat_instance", true)
        .or_default(false);
  }
};

}  // namespace tgbm::api
