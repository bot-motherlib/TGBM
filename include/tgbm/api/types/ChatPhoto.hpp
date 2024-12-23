#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*This object represents a chat photo.*/
struct ChatPhoto {
  /* File identifier of small (160x160) chat photo. This file_id can be used only for photo download and only
   * for as long as the photo is not changed. */
  String small_file_id;
  /* Unique file identifier of small (160x160) chat photo, which is supposed to be the same over time and for
   * different bots. Can't be used to download or reuse the file. */
  String small_file_unique_id;
  /* File identifier of big (640x640) chat photo. This file_id can be used only for photo download and only
   * for as long as the photo is not changed. */
  String big_file_id;
  /* Unique file identifier of big (640x640) chat photo, which is supposed to be the same over time and for
   * different bots. Can't be used to download or reuse the file. */
  String big_file_unique_id;

  constexpr static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name)
        .case_("small_file_id", true)
        .case_("small_file_unique_id", true)
        .case_("big_file_id", true)
        .case_("big_file_unique_id", true)
        .or_default(false);
  }
};

}  // namespace tgbm::api
