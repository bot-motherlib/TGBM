#pragma once

#include "all_fwd.hpp"

namespace tgbm::api {

/*This object represents a service message about a video chat scheduled in the chat.*/
struct VideoChatScheduled {
  /* Point in time (Unix timestamp) when the video chat is supposed to be started by a chat administrator */
  Integer start_date;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("start_date", true).or_default(false);
  }
};

}  // namespace tgbm::api
