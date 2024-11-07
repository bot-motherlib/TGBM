#pragma once

#include "all_fwd.hpp"

namespace tgbm::api {

/*This object contains information about one member of a chat. Currently, the following 6 types of chat
 * members are supported:*/
struct ChatMemberOwner {
  /* Information about the user */
  box<User> user;
  /* True, if the user's presence in the chat is hidden */
  bool is_anonymous;
  /* Optional. Custom title for this user */
  optional<String> custom_title;

  consteval static bool is_mandatory_field(std::string_view name) {
    return utils::string_switch<bool>(name).case_("user", true).case_("is_anonymous", true).or_default(false);
  }
};

}  // namespace tgbm::api
