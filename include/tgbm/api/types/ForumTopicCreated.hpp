#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*This object represents a service message about a new forum topic created in the chat.*/
struct ForumTopicCreated {
  /* Name of the topic */
  String name;
  /* Color of the topic icon in RGB format */
  Integer icon_color;
  /* Optional. Unique identifier of the custom emoji shown as the topic icon */
  optional<String> icon_custom_emoji_id;

  constexpr static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("name", true).case_("icon_color", true).or_default(false);
  }
};

}  // namespace tgbm::api
