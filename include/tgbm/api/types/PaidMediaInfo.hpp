#pragma once

#include "all_fwd.hpp"

namespace tgbm::api {

/*Describes the paid media added to a message.*/
struct PaidMediaInfo {
  /* The number of Telegram Stars that must be paid to buy access to the media */
  Integer star_count;
  /* Information about the paid media */
  arrayof<PaidMedia> paid_media;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("star_count", true).case_("paid_media", true).or_default(false);
  }
};

}  // namespace tgbm::api
