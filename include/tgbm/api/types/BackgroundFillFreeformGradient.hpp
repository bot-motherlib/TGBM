#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*The background is a freeform gradient that rotates after every message in the chat.*/
struct BackgroundFillFreeformGradient {
  /* A list of the 3 or 4 base colors that are used to generate the freeform gradient in the RGB24 format */
  arrayof<Integer> colors;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("colors", true).or_default(false);
  }
};

}  // namespace tgbm::api
