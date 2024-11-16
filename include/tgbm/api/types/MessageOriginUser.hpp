#pragma once

#include "all_fwd.hpp"

namespace tgbm::api {

/*This object describes the origin of a message. It can be one of*/
struct MessageOriginUser {
  /* Date the message was sent originally in Unix time */
  Integer date;
  /* User that sent the message originally */
  box<User> sender_user;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("date", true).case_("sender_user", true).or_default(false);
  }
};

}  // namespace tgbm::api
