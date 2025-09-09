#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*Contains information about a suggested post.*/
struct SuggestedPostInfo {
  /* State of the suggested post. Currently, it can be one of “pending”, “approved”, “declined”. */
  String state;
  /* Optional. Proposed price of the post. If the field is omitted, then the post is unpaid. */
  box<SuggestedPostPrice> price;
  /* Optional. Proposed send date of the post. If the field is omitted, then the post can be published at any
   * time within 30 days at the sole discretion of the user or administrator who approves it. */
  optional<Integer> send_date;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("state", true).or_default(false);
  }
};

}  // namespace tgbm::api
