#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*This object describes the content of a story to post. Currently, it can be one of*/
struct InputStoryContentVideo {
  /* The video to post as a story. The video must be of the size 720x1280, streamable, encoded with H.265
   * codec, with key frames added each second in the MPEG4 format, and must not exceed 30 MB. The video can't
   * be reused and can only be uploaded as a new file, so you can pass “attach://<file_attach_name>” if the
   * video was uploaded using multipart/form-data under <file_attach_name>. More information on Sending Files
   * » */
  String video;
  /* Optional. Precise duration of the video in seconds; 0-60 */
  optional<Double> duration;
  /* Optional. Timestamp in seconds of the frame that will be used as the static cover for the story. Defaults
   * to 0.0. */
  optional<Double> cover_frame_timestamp;
  /* Optional. Pass True if the video has no sound */
  optional<bool> is_animation;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("video", true).or_default(false);
  }
};

}  // namespace tgbm::api
