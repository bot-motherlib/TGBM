#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*The paid media is a photo.*/
struct PaidMediaPhoto {
  /* The photo */
  arrayof<PhotoSize> photo;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("photo", true).or_default(false);
  }
};

}  // namespace tgbm::api
