#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*This object represents a rich formatted text. Currently, it can be either a String for plain text, an Array
 * of RichText, or any of the following types:*/
struct RichTextMathematicalExpression {
  /* The expression in LaTeX format */
  String expression;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("expression", true).or_default(false);
  }
};

}  // namespace tgbm::api
