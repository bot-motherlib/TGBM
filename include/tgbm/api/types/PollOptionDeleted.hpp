#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*Describes a service message about an option deleted from a poll.*/
struct PollOptionDeleted {
  /* Unique identifier of the deleted option */
  String option_persistent_id;
  /* Option text */
  String option_text;
  /* Optional. Message containing the poll from which the option was deleted, if known. Note that the Message
   * object in this field will not contain the reply_to_message field even if it itself is a reply. */
  box<MaybeInaccessibleMessage> poll_message;
  /* Optional. Special entities that appear in the option_text */
  optional<arrayof<MessageEntity>> option_text_entities;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name)
        .case_("option_persistent_id", true)
        .case_("option_text", true)
        .or_default(false);
  }
};

}  // namespace tgbm::api
