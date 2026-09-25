#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*A block with a video, corresponding to the HTML tag <video>.*/
struct RichBlockVideo {
  /* The video */
  box<Video> video;
  /* Optional. Caption of the block */
  box<RichBlockCaption> caption;
  /* Optional. True, if the media preview is covered by a spoiler animation */
  optional<True> has_spoiler;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("video", true).or_default(false);
  }
};

}  // namespace tgbm::api
