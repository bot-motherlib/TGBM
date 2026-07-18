#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*Represents a community (a group of chats).*/
struct Community {
  /* Unique identifier for this community. This number may have more than 32 significant bits and some
   * programming languages may have difficulty/silent defects in interpreting it. But it has at most 52
   * significant bits, so a signed 64-bit integer or double-precision float type are safe for storing this
   * identifier. */
  Integer id;
  /* Name of the community */
  String name;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("id", true).case_("name", true).or_default(false);
  }
};

}  // namespace tgbm::api
