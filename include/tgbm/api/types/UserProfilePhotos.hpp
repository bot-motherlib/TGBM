#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*This object represent a user's profile pictures.*/
struct UserProfilePhotos {
  /* Total number of profile pictures the target user has */
  Integer total_count;
  /* Requested profile pictures (in up to 4 sizes each) */
  arrayof<arrayof<PhotoSize>> photos;

  constexpr static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("total_count", true).case_("photos", true).or_default(false);
  }
};

}  // namespace tgbm::api
