#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*The background is filled using the selected color.*/
struct BackgroundFillSolid {
  /* The color of the background fill in the RGB24 format */
  Integer color;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("color", true).or_default(false);
  }
};

}  // namespace tgbm::api
