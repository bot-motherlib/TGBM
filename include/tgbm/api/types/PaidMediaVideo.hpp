#pragma once

#include "all_fwd.hpp"

namespace tgbm::api {

/*This object describes paid media. Currently, it can be one of*/
struct PaidMediaVideo {
  /* The video */
  box<Video> video;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("video", true).or_default(false);
  }
};

}  // namespace tgbm::api
