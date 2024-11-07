#pragma once

#include "all_fwd.hpp"

namespace tgbm::api {

/*This object contains information about a paid media purchase.*/
struct PaidMediaPurchased {
  /* User who purchased the media */
  box<User> from;
  /* Bot-specified paid media payload */
  String paid_media_payload;

  consteval static bool is_mandatory_field(std::string_view name) {
    return utils::string_switch<bool>(name)
        .case_("from", true)
        .case_("paid_media_payload", true)
        .or_default(false);
  }
};

}  // namespace tgbm::api
