#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*The paid media is a video.*/
struct PaidMediaVideo {
  /* The video */
  box<Video> video;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("video", true).or_default(false);
  }

  bool operator==(const PaidMediaVideo&) const;
  std::strong_ordering operator<=>(const PaidMediaVideo&) const;
};

}  // namespace tgbm::api
