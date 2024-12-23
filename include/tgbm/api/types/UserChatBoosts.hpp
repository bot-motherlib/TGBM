#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*This object represents a list of boosts added to a chat by a user.*/
struct UserChatBoosts {
  /* The list of boosts added to the chat by the user */
  arrayof<ChatBoost> boosts;

  constexpr static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("boosts", true).or_default(false);
  }
};

}  // namespace tgbm::api
