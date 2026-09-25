#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*A block quotation, corresponding to the HTML tag <blockquote>.*/
struct RichBlockBlockQuotation {
  /* Content of the block */
  arrayof<RichBlock> blocks;
  /* Optional. Credit of the block */
  box<RichText> credit;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("blocks", true).or_default(false);
  }
};

}  // namespace tgbm::api
