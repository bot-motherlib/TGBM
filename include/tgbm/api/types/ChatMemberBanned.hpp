#pragma once

#include "all_fwd.hpp"

namespace tgbm::api {

/*This object contains information about one member of a chat. Currently, the following 6 types of chat
 * members are supported:*/
struct ChatMemberBanned {
  /* Information about the user */
  box<User> user;
  /* Date when restrictions will be lifted for this user; Unix time. If 0, then the user is banned forever */
  Integer until_date;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("user", true).case_("until_date", true).or_default(false);
  }
};

}  // namespace tgbm::api
