#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*A block with a mathematical expression in LaTeX format, corresponding to the custom HTML tag
 * <tg-math-block>.*/
struct RichBlockMathematicalExpression {
  /* The mathematical expression in LaTeX format */
  String expression;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("expression", true).or_default(false);
  }
};

}  // namespace tgbm::api
