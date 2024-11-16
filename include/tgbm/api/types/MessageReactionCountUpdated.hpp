#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*This object represents reaction changes on a message with anonymous reactions.*/
struct MessageReactionCountUpdated {
  /* The chat containing the message */
  box<Chat> chat;
  /* Unique message identifier inside the chat */
  Integer message_id;
  /* Date of the change in Unix time */
  Integer date;
  /* List of reactions that are present on the message */
  arrayof<ReactionCount> reactions;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name)
        .case_("chat", true)
        .case_("message_id", true)
        .case_("date", true)
        .case_("reactions", true)
        .or_default(false);
  }
};

}  // namespace tgbm::api
