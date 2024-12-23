#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*This object describes the position on faces where a mask should be placed by default.*/
struct MaskPosition {
  /* The part of the face relative to which the mask should be placed. One of “forehead”, “eyes”, “mouth”, or
   * “chin”. */
  String point;
  /* Shift by X-axis measured in widths of the mask scaled to the face size, from left to right. For example,
   * choosing -1.0 will place mask just to the left of the default mask position. */
  Double x_shift;
  /* Shift by Y-axis measured in heights of the mask scaled to the face size, from top to bottom. For
   * example, 1.0 will place the mask just below the default mask position. */
  Double y_shift;
  /* Mask scaling coefficient. For example, 2.0 means double size. */
  Double scale;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name)
        .case_("point", true)
        .case_("x_shift", true)
        .case_("y_shift", true)
        .case_("scale", true)
        .or_default(false);
  }
};

}  // namespace tgbm::api
