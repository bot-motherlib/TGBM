#pragma once

#include "all_fwd.hpp"

namespace tgbm::api {

/*This object describes the way a background is filled based on the selected colors. Currently, it can be one
 * of*/
struct BackgroundFillSolid {
  /* The color of the background fill in the RGB24 format */
  Integer color;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("color", true).or_default(false);
  }
};

}  // namespace tgbm::api
