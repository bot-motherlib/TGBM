#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*This object describes the type of a background. Currently, it can be one of*/
struct BackgroundTypePattern {
  /* Document with the pattern */
  box<Document> document;
  /* The background fill that is combined with the pattern */
  box<BackgroundFill> fill;
  /* Intensity of the pattern when it is shown above the filled background; 0-100 */
  Integer intensity;
  /* Optional. True, if the background fill must be applied only to the pattern itself. All other pixels are
   * black in this case. For dark themes only */
  optional<True> is_inverted;
  /* Optional. True, if the background moves slightly when the device is tilted */
  optional<True> is_moving;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name)
        .case_("document", true)
        .case_("fill", true)
        .case_("intensity", true)
        .or_default(false);
  }
};

}  // namespace tgbm::api
