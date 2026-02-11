#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*Describes a service message about the chat owner leaving the chat.*/
struct ChatOwnerLeft {
  /* Optional. The user which will be the new owner of the chat if the previous owner does not return to the
   * chat */
  box<User> new_owner;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).or_default(false);
  }
};

}  // namespace tgbm::api
