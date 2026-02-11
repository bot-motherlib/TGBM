#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*This object represents the audios displayed on a user's profile.*/
struct UserProfileAudios {
  /* Total number of profile audios for the target user */
  Integer total_count;
  /* Requested profile audios */
  arrayof<Audio> audios;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("total_count", true).case_("audios", true).or_default(false);
  }
};

}  // namespace tgbm::api
