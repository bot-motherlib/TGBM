#pragma once

#include "all_fwd.hpp"

namespace tgbm::api {

/*This object represents one size of a photo or a file / sticker thumbnail.*/
struct PhotoSize {
  /* Identifier for this file, which can be used to download or reuse the file */
  String file_id;
  /* Unique identifier for this file, which is supposed to be the same over time and for different bots. Can't
   * be used to download or reuse the file. */
  String file_unique_id;
  /* Photo width */
  Integer width;
  /* Photo height */
  Integer height;
  /* Optional. File size in bytes */
  optional<Integer> file_size;

  consteval static bool is_mandatory_field(std::string_view name) {
    return utils::string_switch<bool>(name)
        .case_("file_id", true)
        .case_("file_unique_id", true)
        .case_("width", true)
        .case_("height", true)
        .or_default(false);
  }
};

}  // namespace tgbm::api
