#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*This object describes the content of a story to post. Currently, it can be one of*/
struct InputStoryContentPhoto {
  /* The photo to post as a story. The photo must be of the size 1080x1920 and must not exceed 10 MB. The
   * photo can't be reused and can only be uploaded as a new file, so you can pass
   * “attach://<file_attach_name>” if the photo was uploaded using multipart/form-data under
   * <file_attach_name>. More information on Sending Files » */
  String photo;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("photo", true).or_default(false);
  }
};

}  // namespace tgbm::api
