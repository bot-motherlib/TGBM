#pragma once

#include "all_fwd.hpp"

namespace tgbm::api {

/*This object represents a service message about a video chat ended in the chat.*/
struct VideoChatEnded {
  /* Video chat duration in seconds */
  Integer duration;

  consteval static bool is_mandatory_field(std::string_view name) {
    return utils::string_switch<bool>(name).case_("duration", true).or_default(false);
  }
};

}  // namespace tgbm::api
