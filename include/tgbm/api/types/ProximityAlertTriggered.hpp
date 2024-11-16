#pragma once

#include "all_fwd.hpp"

namespace tgbm::api {

/*This object represents the content of a service message, sent whenever a user in the chat triggers a
 * proximity alert set by another user.*/
struct ProximityAlertTriggered {
  /* User that triggered the alert */
  box<User> traveler;
  /* User that set the alert */
  box<User> watcher;
  /* The distance between the users */
  Integer distance;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name)
        .case_("traveler", true)
        .case_("watcher", true)
        .case_("distance", true)
        .or_default(false);
  }
};

}  // namespace tgbm::api
