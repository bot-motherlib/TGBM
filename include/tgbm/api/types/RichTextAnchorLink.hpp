#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*A link to an anchor.*/
struct RichTextAnchorLink {
  /* The link text */
  box<RichText> text;
  /* The name of the anchor. If the name is empty, then the link brings back to the top of the message. */
  String anchor_name;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("text", true).case_("anchor_name", true).or_default(false);
  }
};

}  // namespace tgbm::api
