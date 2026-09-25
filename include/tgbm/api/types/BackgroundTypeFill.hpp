#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*The background is automatically filled based on the selected colors.*/
struct BackgroundTypeFill {
  /* The background fill */
  box<BackgroundFill> fill;
  /* Dimming of the background in dark themes, as a percentage; 0-100 */
  Integer dark_theme_dimming;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("fill", true).case_("dark_theme_dimming", true).or_default(false);
  }
};

}  // namespace tgbm::api
