#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*Represents a chat member that was banned in the chat and can't return to the chat or view chat messages.*/
struct ChatMemberBanned {
  /* Information about the user */
  box<User> user;
  /* Date when restrictions will be lifted for this user; Unix time. If 0, then the user is banned forever. */
  Integer until_date;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("user", true).case_("until_date", true).or_default(false);
  }
};

}  // namespace tgbm::api
