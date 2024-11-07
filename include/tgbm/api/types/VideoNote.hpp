#pragma once

#include "all_fwd.hpp"

namespace tgbm::api {

/*This object represents a video message (available in Telegram apps as of v.4.0).*/
struct VideoNote {
  /* Identifier for this file, which can be used to download or reuse the file */
  String file_id;
  /* Unique identifier for this file, which is supposed to be the same over time and for different bots. Can't
   * be used to download or reuse the file. */
  String file_unique_id;
  /* Video width and height (diameter of the video message) as defined by the sender */
  Integer length;
  /* Duration of the video in seconds as defined by the sender */
  Integer duration;
  /* Optional. Video thumbnail */
  box<PhotoSize> thumbnail;
  /* Optional. File size in bytes */
  optional<Integer> file_size;

  consteval static bool is_mandatory_field(std::string_view name) {
    return utils::string_switch<bool>(name)
        .case_("file_id", true)
        .case_("file_unique_id", true)
        .case_("length", true)
        .case_("duration", true)
        .or_default(false);
  }
};

}  // namespace tgbm::api
