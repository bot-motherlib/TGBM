#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*This object describes the way a background is filled based on the selected colors. Currently, it can be one
 * of*/
struct BackgroundFillFreeformGradient {
  /* A list of the 3 or 4 base colors that are used to generate the freeform gradient in the RGB24 format */
  arrayof<Integer> colors;

  constexpr static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("colors", true).or_default(false);
  }
};

}  // namespace tgbm::api
