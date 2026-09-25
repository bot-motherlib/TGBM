#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*A block with a photo, corresponding to the HTML tag <img>.*/
struct InputRichBlockPhoto {
  /* The photo. Caption is ignored. */
  box<InputMediaPhoto> photo;
  /* Optional. Caption of the block */
  box<RichBlockCaption> caption;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("photo", true).or_default(false);
  }
};

}  // namespace tgbm::api
