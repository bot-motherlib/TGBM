#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*This object describes the origin of a message. It can be one of*/
struct MessageOriginHiddenUser {
  /* Date the message was sent originally in Unix time */
  Integer date;
  /* Name of the user that sent the message originally */
  String sender_user_name;

  constexpr static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("date", true).case_("sender_user_name", true).or_default(false);
  }
};

}  // namespace tgbm::api
