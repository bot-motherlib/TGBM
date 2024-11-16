#pragma once

#include "all_fwd.hpp"

namespace tgbm::api {

/*This object represents a service message about a forum topic closed in the chat. Currently holds no
 * information.*/
struct ForumTopicClosed {
  /* Optional. New name of the topic, if it was edited */
  optional<String> name;
  /* Optional. New identifier of the custom emoji shown as the topic icon, if it was edited; an empty string
   * if the icon was removed */
  optional<String> icon_custom_emoji_id;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).or_default(false);
  }
};

}  // namespace tgbm::api
