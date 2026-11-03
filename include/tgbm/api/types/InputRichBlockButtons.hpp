#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*A block containing a list of buttons that are shown in one row, corresponding to the custom HTML tag
 * <tg-button-row>.*/
struct InputRichBlockButtons {
  /* List of 1-8 buttons to send */
  arrayof<RichMessageButton> buttons;
  /* Optional. Horizontal alignment of the buttons. Currently, must be one of “left”, “center”, or “right”. */
  optional<String> align;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("buttons", true).or_default(false);
  }
};

}  // namespace tgbm::api
