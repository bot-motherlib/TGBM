#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*MISSING DESCRIPTION ADDED FOR GENERATION*/
struct EphemeralMessageParameters {
  /* Identifier of the user who will receive the message. It is not guaranteed that the user will receive the
   * message, especially if they are offline. See here for more details. */
  Integer receiver_user_id;
  /* Optional. Identifier of the callback query which triggered the message, if any */
  optional<String> callback_query_id;
  /* Optional. Pass True if the ephemeral message must be shown in place of the original message. Must be
   * False for callback queries from ephemeral messages, which must be edited using regular
   * editEphemeralMessage… methods. */
  optional<bool> replace_callback_query_message;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("receiver_user_id", true).or_default(false);
  }
};

}  // namespace tgbm::api
