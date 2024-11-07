#pragma once

#include "all_fwd.hpp"

namespace tgbm::api {

/*This object contains information about the users whose identifiers were shared with the bot using a
 * KeyboardButtonRequestUsers button.*/
struct UsersShared {
  /* Identifier of the request */
  Integer request_id;
  /* Information about users shared with the bot. */
  arrayof<SharedUser> users;

  consteval static bool is_mandatory_field(std::string_view name) {
    return utils::string_switch<bool>(name).case_("request_id", true).case_("users", true).or_default(false);
  }
};

}  // namespace tgbm::api
