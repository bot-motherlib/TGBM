#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*Describes the position of a clickable area within a story.*/
struct StoryAreaPosition {
  /* The abscissa of the area's center, as a percentage of the media width */
  Double x_percentage;
  /* The ordinate of the area's center, as a percentage of the media height */
  Double y_percentage;
  /* The width of the area's rectangle, as a percentage of the media width */
  Double width_percentage;
  /* The height of the area's rectangle, as a percentage of the media height */
  Double height_percentage;
  /* The clockwise rotation angle of the rectangle, in degrees; 0-360 */
  Double rotation_angle;
  /* The radius of the rectangle corner rounding, as a percentage of the media width */
  Double corner_radius_percentage;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name)
        .case_("x_percentage", true)
        .case_("y_percentage", true)
        .case_("width_percentage", true)
        .case_("height_percentage", true)
        .case_("rotation_angle", true)
        .case_("corner_radius_percentage", true)
        .or_default(false);
  }
};

}  // namespace tgbm::api
