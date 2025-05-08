#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*This object describes the types of gifts that can be gifted to a user or a chat.*/
struct AcceptedGiftTypes {
  /* True, if unlimited regular gifts are accepted */
  bool unlimited_gifts;
  /* True, if limited regular gifts are accepted */
  bool limited_gifts;
  /* True, if unique gifts or gifts that can be upgraded to unique for free are accepted */
  bool unique_gifts;
  /* True, if a Telegram Premium subscription is accepted */
  bool premium_subscription;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name)
        .case_("unlimited_gifts", true)
        .case_("limited_gifts", true)
        .case_("unique_gifts", true)
        .case_("premium_subscription", true)
        .or_default(false);
  }
};

}  // namespace tgbm::api
