#pragma once

#include "all_fwd.hpp"

namespace tgbm::api {

/*Describes why a request was unsuccessful.*/
struct ResponseParameters {
  /* Optional. The group has been migrated to a supergroup with the specified identifier. This number may have
   * more than 32 significant bits and some programming languages may have difficulty/silent defects in
   * interpreting it. But it has at most 52 significant bits, so a signed 64-bit integer or double-precision
   * float type are safe for storing this identifier. */
  optional<Integer> migrate_to_chat_id;
  /* Optional. In case of exceeding flood control, the number of seconds left to wait before the request can
   * be repeated */
  optional<Integer> retry_after;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).or_default(false);
  }
};

}  // namespace tgbm::api
