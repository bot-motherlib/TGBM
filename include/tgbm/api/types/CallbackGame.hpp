#pragma once

#include "all_fwd.hpp"

namespace tgbm::api {

/*A placeholder, currently holds no information. Use BotFather to set up your game.*/
struct CallbackGame {
  /* Yes */
  Integer user_id;
  /* Yes */
  Integer score;
  /* Optional */
  optional<Integer> chat_id;
  /* Optional */
  optional<Integer> message_id;
  /* Optional */
  optional<String> inline_message_id;
  /* Optional */
  optional<bool> force;
  /* Optional */
  optional<bool> disable_edit_message;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("user_id", true).case_("score", true).or_default(false);
  }
};

}  // namespace tgbm::api
