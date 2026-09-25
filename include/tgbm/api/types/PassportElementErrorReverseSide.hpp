#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*Represents an issue with the reverse side of a document. The error is considered resolved when the file with
 * reverse side of the document changes.*/
struct PassportElementErrorReverseSide {
  /* The section of the user's Telegram Passport which has the issue, one of “driver_license”, “identity_card”
   */
  String type;
  /* Base64-encoded hash of the file with the reverse side of the document */
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
