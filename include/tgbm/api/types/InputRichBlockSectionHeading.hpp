#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*A section heading, corresponding to the HTML tags <h1>, <h2>, <h3>, <h4>, <h5>, or <h6>.*/
struct InputRichBlockSectionHeading {
  /* Text of the block */
  box<RichText> text;
  /* Relative size of the text font; 1-6, 1 is the largest, 6 is the smallest */
  Integer size;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("text", true).case_("size", true).or_default(false);
  }
};

}  // namespace tgbm::api
