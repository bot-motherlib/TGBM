#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*This object defines the parameters for the creation of a managed bot. Information about the created bot will
 * be shared with the bot using the update managed_bot and a Message with the field managed_bot_created.*/
struct KeyboardButtonRequestManagedBot {
  /* Signed 32-bit identifier of the request. Must be unique within the message */
  Integer request_id;
  /* Optional. Suggested name for the bot */
  optional<String> suggested_name;
  /* Optional. Suggested username for the bot */
  optional<String> suggested_username;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("request_id", true).or_default(false);
  }
};

}  // namespace tgbm::api
