#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*This object describes paid media. Currently, it can be one of*/
struct PaidMediaPhoto {
  /* The photo */
  arrayof<PhotoSize> photo;

  constexpr static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("photo", true).or_default(false);
  }
};

}  // namespace tgbm::api
