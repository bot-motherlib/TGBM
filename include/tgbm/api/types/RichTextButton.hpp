#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*A button.*/
struct RichTextButton {
  /* The button */
  box<RichMessageButton> button;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("button", true).or_default(false);
  }
};

}  // namespace tgbm::api
