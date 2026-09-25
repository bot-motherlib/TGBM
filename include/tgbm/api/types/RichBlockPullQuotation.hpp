#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*A quotation with centered text, loosely corresponding to the HTML tag <aside>.*/
struct RichBlockPullQuotation {
  /* Text of the block */
  box<RichText> text;
  /* Optional. Credit of the block */
  box<RichText> credit;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("text", true).or_default(false);
  }
};

}  // namespace tgbm::api
