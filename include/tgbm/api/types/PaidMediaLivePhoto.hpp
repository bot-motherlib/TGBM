#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*The paid media is a live photo.*/
struct PaidMediaLivePhoto {
  /* The photo */
  box<LivePhoto> live_photo;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("live_photo", true).or_default(false);
  }
};

}  // namespace tgbm::api
