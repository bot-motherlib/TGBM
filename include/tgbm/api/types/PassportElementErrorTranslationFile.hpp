#pragma once

#include "all_fwd.hpp"

namespace tgbm::api {

/*This object represents an error in the Telegram Passport element which was submitted that should be resolved
 * by the user. It should be one of:*/
struct PassportElementErrorTranslationFile {
  /* Type of element of the user's Telegram Passport which has the issue, one of “passport”, “driver_license”,
   * “identity_card”, “internal_passport”, “utility_bill”, “bank_statement”, “rental_agreement”,
   * “passport_registration”, “temporary_registration” */
  String type;
  /* Base64-encoded file hash */
  String file_hash;
  /* Error message */
  String message;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name)
        .case_("type", true)
        .case_("file_hash", true)
        .case_("message", true)
        .or_default(false);
  }
};

}  // namespace tgbm::api
