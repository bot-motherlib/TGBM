#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*A collage, corresponding to the custom HTML tag <tg-collage>.*/
struct InputRichBlockCollage {
  /* Elements of the collage */
  arrayof<InputRichBlock> blocks;
  /* Optional. Caption of the block */
  box<RichBlockCaption> caption;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("blocks", true).or_default(false);
  }
};

}  // namespace tgbm::api
