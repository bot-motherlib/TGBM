#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*This object represents a file uploaded to Telegram Passport. Currently all Telegram Passport files are in
 * JPEG format when decrypted and don't exceed 10MB.*/
struct PassportFile {
  /* Identifier for this file, which can be used to download or reuse the file */
  String file_id;
  /* Unique identifier for this file, which is supposed to be the same over time and for different bots. Can't
   * be used to download or reuse the file. */
  String file_unique_id;
  /* File size in bytes */
  Integer file_size;
  /* Unix time when the file was uploaded */
  Integer file_date;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name)
        .case_("file_id", true)
        .case_("file_unique_id", true)
        .case_("file_size", true)
        .case_("file_date", true)
        .or_default(false);
  }
};

}  // namespace tgbm::api
