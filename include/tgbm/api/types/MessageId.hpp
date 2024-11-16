#pragma once

#include "all_fwd.hpp"

namespace tgbm::api {

/*This object represents a unique message identifier.*/
struct MessageId {
  /* Unique message identifier. In specific instances (e.g., message containing a video sent to a big chat),
   * the server might automatically schedule a message instead of sending it immediately. In such cases, this
   * field will be 0 and the relevant message will be unusable until it is actually sent */
  Integer message_id;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("message_id", true).or_default(false);
  }
};

}  // namespace tgbm::api
