#pragma once

#include "all_fwd.hpp"

namespace tgbm::api {

/*This object represents type of a poll, which is allowed to be created and sent when the corresponding button
 * is pressed.*/
struct KeyboardButtonPollType {
  /* Optional. If quiz is passed, the user will be allowed to create only polls in the quiz mode. If regular
   * is passed, only regular polls will be allowed. Otherwise, the user will be allowed to create a poll of
   * any type. */
  optional<String> type;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).or_default(false);
  }
};

}  // namespace tgbm::api
