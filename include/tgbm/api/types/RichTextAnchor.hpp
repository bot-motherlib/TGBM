#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*An anchor.*/
struct RichTextAnchor {
  /* The name of the anchor */
  String name;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("name", true).or_default(false);
  }
};

}  // namespace tgbm::api
