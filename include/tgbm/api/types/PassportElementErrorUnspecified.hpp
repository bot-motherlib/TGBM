#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*Represents an issue in an unspecified place. The error is considered resolved when new data is added.*/
struct PassportElementErrorUnspecified {
  /* Type of element of the user's Telegram Passport which has the issue */
  String type;
  /* Base64-encoded element hash */
  String element_hash;
  /* Error message */
  String message;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name)
        .case_("type", true)
        .case_("element_hash", true)
        .case_("message", true)
        .or_default(false);
  }
};

}  // namespace tgbm::api
