#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*A marked text.*/
struct RichTextMarked {
  /* The text */
  box<RichText> text;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("text", true).or_default(false);
  }
};

}  // namespace tgbm::api
