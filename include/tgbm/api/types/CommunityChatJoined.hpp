#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*Describes a service message about a chat being joined by a user from a community.*/
struct CommunityChatJoined {
  /* The community from which the chat was joined */
  box<Community> community;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("community", true).or_default(false);
  }
};

}  // namespace tgbm::api
