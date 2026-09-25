#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*The boost was obtained by subscribing to Telegram Premium or by gifting a Telegram Premium subscription to
 * another user.*/
struct ChatBoostSourcePremium {
  /* User that boosted the chat */
  box<User> user;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("user", true).or_default(false);
  }
};

}  // namespace tgbm::api
