#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*This object represents a rich formatted text. Currently, it can be either a String for plain text, an Array
 * of RichText, or any of the following types:*/
struct RichTextUnderline {
  /* The text */
  box<RichText> text;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("text", true).or_default(false);
  }
};

}  // namespace tgbm::api
