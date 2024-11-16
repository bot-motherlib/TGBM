#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*This object describes the way a background is filled based on the selected colors. Currently, it can be one
 * of*/
struct BackgroundFillGradient {
  /* Top color of the gradient in the RGB24 format */
  Integer top_color;
  /* Bottom color of the gradient in the RGB24 format */
  Integer bottom_color;
  /* Clockwise rotation angle of the background fill in degrees; 0-359 */
  Integer rotation_angle;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name)
        .case_("top_color", true)
        .case_("bottom_color", true)
        .case_("rotation_angle", true)
        .or_default(false);
  }
};

}  // namespace tgbm::api
