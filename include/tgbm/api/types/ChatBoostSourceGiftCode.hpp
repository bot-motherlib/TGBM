#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*This object describes the source of a chat boost. It can be one of*/
struct ChatBoostSourceGiftCode {
  /* User for which the gift code was created */
  box<User> user;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("user", true).or_default(false);
  }
};

}  // namespace tgbm::api
