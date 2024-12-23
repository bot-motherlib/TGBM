#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*Describes an inline message sent by a Web App on behalf of a user.*/
struct SentWebAppMessage {
  /* Optional. Identifier of the sent inline message. Available only if there is an inline keyboard attached
   * to the message. */
  optional<String> inline_message_id;

  constexpr static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).or_default(false);
  }
};

}  // namespace tgbm::api
