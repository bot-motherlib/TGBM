#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*Caption of a rich formatted block.*/
struct RichBlockCaption {
  /* Block caption */
  box<RichText> text;
  /* Optional. Block credit which corresponds to the HTML tag <cite> */
  box<RichText> credit;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("text", true).or_default(false);
  }
};

}  // namespace tgbm::api
