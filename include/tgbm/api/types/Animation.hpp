#pragma once

#include "all_fwd.hpp"

namespace tgbm::api {

/*This object represents an animation file (GIF or H.264/MPEG-4 AVC video without sound).*/
struct Animation {
  /* Identifier for this file, which can be used to download or reuse the file */
  String file_id;
  /* Unique identifier for this file, which is supposed to be the same over time and for different bots. Can't
   * be used to download or reuse the file. */
  String file_unique_id;
  /* Video width as defined by the sender */
  Integer width;
  /* Video height as defined by the sender */
  Integer height;
  /* Duration of the video in seconds as defined by the sender */
  Integer duration;
  /* Optional. Animation thumbnail as defined by the sender */
  box<PhotoSize> thumbnail;
  /* Optional. Original animation filename as defined by the sender */
  optional<String> file_name;
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
        .case_("width", true)
        .case_("height", true)
        .case_("duration", true)
        .or_default(false);
  }
};

}  // namespace tgbm::api
