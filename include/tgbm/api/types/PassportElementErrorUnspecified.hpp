#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*This object represents an error in the Telegram Passport element which was submitted that should be resolved
 * by the user. It should be one of:*/
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
