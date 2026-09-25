#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*A block with a video, corresponding to the HTML tag <video>.*/
struct InputRichBlockVideo {
  /* The video. Caption is ignored. */
  box<InputMediaVideo> video;
  /* Optional. Caption of the block */
  box<RichBlockCaption> caption;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("video", true).or_default(false);
  }
};

}  // namespace tgbm::api
