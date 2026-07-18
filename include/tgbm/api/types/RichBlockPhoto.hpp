#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*This object represents a block in a rich formatted message. Currently, it can be any of the following
 * types:*/
struct RichBlockPhoto {
  /* Available sizes of the photo */
  arrayof<PhotoSize> photo;
  /* Optional. Caption of the block */
  box<RichBlockCaption> caption;
  /* Optional. True, if the media preview is covered by a spoiler animation */
  optional<True> has_spoiler;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("photo", true).or_default(false);
  }
};

}  // namespace tgbm::api
