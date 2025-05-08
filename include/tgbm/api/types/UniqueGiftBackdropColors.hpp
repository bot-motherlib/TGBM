#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*This object describes the colors of the backdrop of a unique gift.*/
struct UniqueGiftBackdropColors {
  /* The color in the center of the backdrop in RGB format */
  Integer center_color;
  /* The color on the edges of the backdrop in RGB format */
  Integer edge_color;
  /* The color to be applied to the symbol in RGB format */
  Integer symbol_color;
  /* The color for the text on the backdrop in RGB format */
  Integer text_color;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name)
        .case_("center_color", true)
        .case_("edge_color", true)
        .case_("symbol_color", true)
        .case_("text_color", true)
        .or_default(false);
  }
};

}  // namespace tgbm::api
