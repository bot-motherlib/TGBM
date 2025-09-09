#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*Contains parameters of a post that is being suggested by the bot.*/
struct SuggestedPostParameters {
  /* Optional. Proposed price for the post. If the field is omitted, then the post is unpaid. */
  box<SuggestedPostPrice> price;
  /* Optional. Proposed send date of the post. If specified, then the date must be between 300 second and
   * 2678400 seconds (30 days) in the future. If the field is omitted, then the post can be published at any
   * time within 30 days at the sole discretion of the user who approves it. */
  optional<Integer> send_date;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).or_default(false);
  }
};

}  // namespace tgbm::api
