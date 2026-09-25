#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*A placeholder, currently holds no information. Use BotFather to set up your game.*/
struct CallbackGame {
  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).or_default(false);
  }
};

}  // namespace tgbm::api
