#pragma once

#include "all_fwd.hpp"

namespace tgbm::api {

/*This object describes a message that was deleted or is otherwise inaccessible to the bot.*/
struct InaccessibleMessage {
  /* Chat the message belonged to */
  box<Chat> chat;
  /* Unique message identifier inside the chat */
  Integer message_id;
  /* Always 0. The field can be used to differentiate regular and inaccessible messages. */
  Integer date;

  consteval static bool is_mandatory_field(std::string_view name) {
    return utils::string_switch<bool>(name)
        .case_("chat", true)
        .case_("message_id", true)
        .case_("date", true)
        .or_default(false);
  }
};

}  // namespace tgbm::api
