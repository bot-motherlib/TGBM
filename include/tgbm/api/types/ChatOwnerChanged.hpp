#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*Describes a service message about an ownership change in the chat.*/
struct ChatOwnerChanged {
  /* The new owner of the chat */
  box<User> new_owner;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("new_owner", true).or_default(false);
  }
};

}  // namespace tgbm::api
