#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*This object represents a service message about new members invited to a video chat.*/
struct VideoChatParticipantsInvited {
  /* New members that were invited to the video chat */
  arrayof<User> users;

  constexpr static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("users", true).or_default(false);
  }
};

}  // namespace tgbm::api
