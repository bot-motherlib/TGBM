#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*This object contains information about the color scheme for a user's name, message replies and link previews
 * based on a unique gift.*/
struct UniqueGiftColors {
  /* Custom emoji identifier of the unique gift's model */
  String model_custom_emoji_id;
  /* Custom emoji identifier of the unique gift's symbol */
  String symbol_custom_emoji_id;
  /* Main color used in light themes; RGB format */
  Integer light_theme_main_color;
  /* List of 1-3 additional colors used in light themes; RGB format */
  arrayof<Integer> light_theme_other_colors;
  /* Main color used in dark themes; RGB format */
  Integer dark_theme_main_color;
  /* List of 1-3 additional colors used in dark themes; RGB format */
  arrayof<Integer> dark_theme_other_colors;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name)
        .case_("model_custom_emoji_id", true)
        .case_("symbol_custom_emoji_id", true)
        .case_("light_theme_main_color", true)
        .case_("light_theme_other_colors", true)
        .case_("dark_theme_main_color", true)
        .case_("dark_theme_other_colors", true)
        .or_default(false);
  }
};

}  // namespace tgbm::api
