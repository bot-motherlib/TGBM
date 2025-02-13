#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*This object describes the paid media to be sent. Currently, it can be one of*/
struct InputPaidMediaVideo {
  /* File to send. Pass a file_id to send a file that exists on the Telegram servers (recommended), pass an
   * HTTP URL for Telegram to get a file from the Internet, or pass “attach://<file_attach_name>” to upload a
   * new one using multipart/form-data under <file_attach_name> name. More information on Sending Files » */
  String media;
  /* Optional. Thumbnail of the file sent; can be ignored if thumbnail generation for the file is supported
   * server-side. The thumbnail should be in JPEG format and less than 200 kB in size. A thumbnail's width and
   * height should not exceed 320. Ignored if the file is not uploaded using multipart/form-data. Thumbnails
   * can't be reused and can be only uploaded as a new file, so you can pass “attach://<file_attach_name>” if
   * the thumbnail was uploaded using multipart/form-data under <file_attach_name>. More information on
   * Sending Files » */
  optional<String> thumbnail;
  /* Optional. Cover for the video in the message. Pass a file_id to send a file that exists on the Telegram
   * servers (recommended), pass an HTTP URL for Telegram to get a file from the Internet, or pass
   * “attach://<file_attach_name>” to upload a new one using multipart/form-data under <file_attach_name>
   * name. More information on Sending Files » */
  optional<String> cover;
  /* Optional. Start timestamp for the video in the message */
  optional<Integer> start_timestamp;
  /* Optional. Video width */
  optional<Integer> width;
  /* Optional. Video height */
  optional<Integer> height;
  /* Optional. Video duration in seconds */
  optional<Integer> duration;
  /* Optional. Pass True if the uploaded video is suitable for streaming */
  optional<bool> supports_streaming;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("media", true).or_default(false);
  }
};

}  // namespace tgbm::api
