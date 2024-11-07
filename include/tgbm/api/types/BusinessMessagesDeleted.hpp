#pragma once

#include "all_fwd.hpp"

namespace tgbm::api {

/*This object is received when messages are deleted from a connected business account.*/
struct BusinessMessagesDeleted {
  /* Unique identifier of the business connection */
  String business_connection_id;
  /* Information about a chat in the business account. The bot may not have access to the chat or the
   * corresponding user. */
  box<Chat> chat;
  /* The list of identifiers of deleted messages in the chat of the business account */
  arrayof<Integer> message_ids;

  consteval static bool is_mandatory_field(std::string_view name) {
    return utils::string_switch<bool>(name)
        .case_("business_connection_id", true)
        .case_("chat", true)
        .case_("message_ids", true)
        .or_default(false);
  }
};

}  // namespace tgbm::api
