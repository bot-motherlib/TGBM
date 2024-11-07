#pragma once

#include "all_fwd.hpp"

namespace tgbm::api {

/*This object contains information about one member of a chat. Currently, the following 6 types of chat
 * members are supported:*/
struct ChatMemberLeft {
  /* Information about the user */
  box<User> user;

  consteval static bool is_mandatory_field(std::string_view name) {
    return utils::string_switch<bool>(name).case_("user", true).or_default(false);
  }
};

}  // namespace tgbm::api
