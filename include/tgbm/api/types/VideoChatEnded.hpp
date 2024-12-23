#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*This object represents a service message about a video chat ended in the chat.*/
struct VideoChatEnded {
  /* Video chat duration in seconds */
  Integer duration;

  constexpr static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("duration", true).or_default(false);
  }
};

}  // namespace tgbm::api
