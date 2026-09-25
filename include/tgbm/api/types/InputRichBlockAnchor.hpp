#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*A block with an anchor, corresponding to the HTML tag <a> with the attribute name.*/
struct InputRichBlockAnchor {
  /* The name of the anchor */
  String name;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("name", true).or_default(false);
  }
};

}  // namespace tgbm::api
