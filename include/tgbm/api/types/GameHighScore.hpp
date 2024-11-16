#pragma once

#include "all_fwd.hpp"

namespace tgbm::api {

/*This object represents one row of the high scores table for a game.*/
struct GameHighScore {
  /* Position in high score table for the game */
  Integer position;
  /* User */
  box<User> user;
  /* Score */
  Integer score;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name)
        .case_("position", true)
        .case_("user", true)
        .case_("score", true)
        .or_default(false);
  }
};

}  // namespace tgbm::api
