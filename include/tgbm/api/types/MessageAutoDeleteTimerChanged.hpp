#pragma once

#include "all_fwd.hpp"

namespace tgbm::api {

/*This object represents a service message about a change in auto-delete timer settings.*/
struct MessageAutoDeleteTimerChanged {
  /* New auto-delete time for messages in the chat; in seconds */
  Integer message_auto_delete_time;

  consteval static bool is_mandatory_field(std::string_view name) {
    return utils::string_switch<bool>(name).case_("message_auto_delete_time", true).or_default(false);
  }
};

}  // namespace tgbm::api
