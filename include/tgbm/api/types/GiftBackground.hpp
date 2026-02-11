#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*This object describes the background of a gift.*/
struct GiftBackground {
  /* Center color of the background in RGB format */
  Integer center_color;
  /* Edge color of the background in RGB format */
  Integer edge_color;
  /* Text color of the background in RGB format */
  Integer text_color;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name)
        .case_("center_color", true)
        .case_("edge_color", true)
        .case_("text_color", true)
        .or_default(false);
  }
};

}  // namespace tgbm::api
