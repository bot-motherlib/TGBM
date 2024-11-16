#pragma once

#include "all_fwd.hpp"

namespace tgbm::api {

/*This object represents an error in the Telegram Passport element which was submitted that should be resolved
 * by the user. It should be one of:*/
struct PassportElementErrorDataField {
  /* The section of the user's Telegram Passport which has the error, one of “personal_details”, “passport”,
   * “driver_license”, “identity_card”, “internal_passport”, “address” */
  String type;
  /* Name of the data field which has the error */
  String field_name;
  /* Base64-encoded data hash */
  String data_hash;
  /* Error message */
  String message;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name)
        .case_("type", true)
        .case_("field_name", true)
        .case_("data_hash", true)
        .case_("message", true)
        .or_default(false);
  }
};

}  // namespace tgbm::api
