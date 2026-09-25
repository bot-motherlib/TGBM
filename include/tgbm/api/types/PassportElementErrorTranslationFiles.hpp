#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*Represents an issue with the translated version of a document. The error is considered resolved when a file
 * with the document translation change.*/
struct PassportElementErrorTranslationFiles {
  /* Type of element of the user's Telegram Passport which has the issue, one of “passport”, “driver_license”,
   * “identity_card”, “internal_passport”, “utility_bill”, “bank_statement”, “rental_agreement”,
   * “passport_registration”, “temporary_registration” */
  String type;
  /* List of base64-encoded file hashes */
  arrayof<String> file_hashes;
  /* Error message */
  String message;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name)
        .case_("type", true)
        .case_("file_hashes", true)
        .case_("message", true)
        .or_default(false);
  }
};

}  // namespace tgbm::api
