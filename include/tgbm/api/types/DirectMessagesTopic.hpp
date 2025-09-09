#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*Describes a topic of a direct messages chat.*/
struct DirectMessagesTopic {
  /* Unique identifier of the topic. This number may have more than 32 significant bits and some programming
   * languages may have difficulty/silent defects in interpreting it. But it has at most 52 significant bits,
   * so a 64-bit integer or double-precision float type are safe for storing this identifier. */
  Integer topic_id;
  /* Optional. Information about the user that created the topic. Currently, it is always present */
  box<User> user;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("topic_id", true).or_default(false);
  }
};

}  // namespace tgbm::api
