#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*This object describes an update about a user stopping message generation.*/
struct MessageGenerationStopped {
  /* Chat in which the message is generated */
  box<Chat> chat;
  /* Unique identifier of the message draft which was stopped */
  Integer draft_id;
  /* Optional. Unique identifier of the message thread in which the message is generated */
  optional<Integer> message_thread_id;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("chat", true).case_("draft_id", true).or_default(false);
  }
};

}  // namespace tgbm::api
