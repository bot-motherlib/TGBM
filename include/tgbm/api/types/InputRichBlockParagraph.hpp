#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*A text paragraph, corresponding to the HTML tag <p>.*/
struct InputRichBlockParagraph {
  /* Text of the block */
  box<RichText> text;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("text", true).or_default(false);
  }
};

}  // namespace tgbm::api
