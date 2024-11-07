#pragma once

#include "all_fwd.hpp"

namespace tgbm::api {

/*This object represents a service message about a video chat started in the chat. Currently holds no
 * information.*/
struct VideoChatStarted {
  /* Video chat duration in seconds */
  Integer duration;

  consteval static bool is_mandatory_field(std::string_view name) {
    return utils::string_switch<bool>(name).case_("duration", true).or_default(false);
  }
};

}  // namespace tgbm::api
