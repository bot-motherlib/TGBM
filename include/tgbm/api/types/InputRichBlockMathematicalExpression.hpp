#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*This object represents a block in a rich formatted message to be sent. Currently, it can be any of the
 * following types:*/
struct InputRichBlockMathematicalExpression {
  /* The mathematical expression in LaTeX format */
  String expression;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("expression", true).or_default(false);
  }
};

}  // namespace tgbm::api
