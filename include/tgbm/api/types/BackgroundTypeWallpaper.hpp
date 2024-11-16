#pragma once

#include "all_fwd.hpp"

namespace tgbm::api {

/*This object describes the type of a background. Currently, it can be one of*/
struct BackgroundTypeWallpaper {
  /* Document with the wallpaper */
  box<Document> document;
  /* Dimming of the background in dark themes, as a percentage; 0-100 */
  Integer dark_theme_dimming;
  /* Optional. True, if the wallpaper is downscaled to fit in a 450x450 square and then box-blurred with
   * radius 12 */
  optional<True> is_blurred;
  /* Optional. True, if the background moves slightly when the device is tilted */
  optional<True> is_moving;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name)
        .case_("document", true)
        .case_("dark_theme_dimming", true)
        .or_default(false);
  }
};

}  // namespace tgbm::api
