#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*This object describes the origin of a message. It can be one of*/
struct MessageOriginChat {
  /* Date the message was sent originally in Unix time */
  Integer date;
  /* Chat that sent the message originally */
  box<Chat> sender_chat;
  /* Optional. For messages originally sent by an anonymous chat administrator, original message author
   * signature */
  optional<String> author_signature;

  constexpr static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("date", true).case_("sender_chat", true).or_default(false);
  }
};

}  // namespace tgbm::api
