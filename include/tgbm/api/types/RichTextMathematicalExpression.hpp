#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*A mathematical expression.*/
struct RichTextMathematicalExpression {
  /* The expression in LaTeX format */
  String expression;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("expression", true).or_default(false);
  }
};

}  // namespace tgbm::api
