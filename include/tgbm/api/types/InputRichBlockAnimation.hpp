#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*A block with an animation, corresponding to the HTML tag <video>.*/
struct InputRichBlockAnimation {
  /* The animation. Caption is ignored. */
  box<InputMediaAnimation> animation;
  /* Optional. Caption of the block */
  box<RichBlockCaption> caption;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("animation", true).or_default(false);
  }
};

}  // namespace tgbm::api
