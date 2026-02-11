#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*This object represents a video file of a specific quality.*/
struct VideoQuality {
  /* Identifier for this file, which can be used to download or reuse the file */
  String file_id;
  /* Unique identifier for this file, which is supposed to be the same over time and for different bots. Can't
   * be used to download or reuse the file. */
  String file_unique_id;
  /* Video width */
  Integer width;
  /* Video height */
  Integer height;
  /* Codec that was used to encode the video, for example, “h264”, “h265”, or “av01” */
  String codec;
  /* Optional. File size in bytes. It can be bigger than 2^31 and some programming languages may have
   * difficulty/silent defects in interpreting it. But it has at most 52 significant bits, so a signed 64-bit
   * integer or double-precision float type are safe for storing this value. */
  optional<Integer> file_size;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name)
        .case_("file_id", true)
        .case_("file_unique_id", true)
        .case_("width", true)
        .case_("height", true)
        .case_("codec", true)
        .or_default(false);
  }
};

}  // namespace tgbm::api
