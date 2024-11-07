#pragma once

#include "all_fwd.hpp"

namespace tgbm::api {

/*This object represents a voice note.*/
struct Voice {
  /* Identifier for this file, which can be used to download or reuse the file */
  String file_id;
  /* Unique identifier for this file, which is supposed to be the same over time and for different bots. Can't
   * be used to download or reuse the file. */
  String file_unique_id;
  /* Duration of the audio in seconds as defined by the sender */
  Integer duration;
  /* Optional. MIME type of the file as defined by the sender */
  optional<String> mime_type;
  /* Optional. File size in bytes. It can be bigger than 2^31 and some programming languages may have
   * difficulty/silent defects in interpreting it. But it has at most 52 significant bits, so a signed 64-bit
   * integer or double-precision float type are safe for storing this value. */
  optional<Integer> file_size;

  consteval static bool is_mandatory_field(std::string_view name) {
    return utils::string_switch<bool>(name)
        .case_("file_id", true)
        .case_("file_unique_id", true)
        .case_("duration", true)
        .or_default(false);
  }
};

}  // namespace tgbm::api