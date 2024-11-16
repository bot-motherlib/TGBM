#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*This object describes the origin of a message. It can be one of*/
struct MessageOriginChannel {
  /* Date the message was sent originally in Unix time */
  Integer date;
  /* Channel chat to which the message was originally sent */
  box<Chat> chat;
  /* Unique message identifier inside the chat */
  Integer message_id;
  /* Optional. Signature of the original post author */
  optional<String> author_signature;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name)
        .case_("date", true)
        .case_("chat", true)
        .case_("message_id", true)
        .or_default(false);
  }
};

}  // namespace tgbm::api
