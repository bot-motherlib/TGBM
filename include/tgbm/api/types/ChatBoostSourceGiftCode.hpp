#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*The boost was obtained by the creation of Telegram Premium gift codes to boost a chat. Each such code boosts
 * the chat 4 times for the duration of the corresponding Telegram Premium subscription.*/
struct ChatBoostSourceGiftCode {
  /* User for which the gift code was created */
  box<User> user;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("user", true).or_default(false);
  }
};

}  // namespace tgbm::api
