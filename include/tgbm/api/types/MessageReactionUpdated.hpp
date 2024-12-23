#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*This object represents a change of a reaction on a message performed by a user.*/
struct MessageReactionUpdated {
  /* The chat containing the message the user reacted to */
  box<Chat> chat;
  /* Unique identifier of the message inside the chat */
  Integer message_id;
  /* Date of the change in Unix time */
  Integer date;
  /* Previous list of reaction types that were set by the user */
  arrayof<ReactionType> old_reaction;
  /* New list of reaction types that have been set by the user */
  arrayof<ReactionType> new_reaction;
  /* Optional. The user that changed the reaction, if the user isn't anonymous */
  box<User> user;
  /* Optional. The chat on behalf of which the reaction was changed, if the user is anonymous */
  box<Chat> actor_chat;

  constexpr static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name)
        .case_("chat", true)
        .case_("message_id", true)
        .case_("date", true)
        .case_("old_reaction", true)
        .case_("new_reaction", true)
        .or_default(false);
  }
};

}  // namespace tgbm::api
