#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*This object describes a profile photo to set. Currently, it can be one of*/
struct InputProfilePhotoAnimated {
  /* The animated profile photo. Profile photos can't be reused and can only be uploaded as a new file, so you
   * can pass “attach://<file_attach_name>” if the photo was uploaded using multipart/form-data under
   * <file_attach_name>. More information on Sending Files » */
  String animation;
  /* Optional. Timestamp in seconds of the frame that will be used as the static profile photo. Defaults to
   * 0.0. */
  optional<Double> main_frame_timestamp;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("animation", true).or_default(false);
  }
};

}  // namespace tgbm::api
