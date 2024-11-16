#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*This object represents an animated emoji that displays a random value.*/
struct Dice {
  /* Emoji on which the dice throw animation is based */
  String emoji;
  /* Value of the dice, 1-6 for “”, “” and “” base emoji, 1-5 for “” and “” base emoji, 1-64 for “” base emoji
   */
  Integer value;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("emoji", true).case_("value", true).or_default(false);
  }
};

}  // namespace tgbm::api
