#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*This object describes the paid media to be sent. Currently, it can be one of*/
struct InputPaidMediaLivePhoto {
  /* Video of the live photo to send. Pass a file_id to send a file that exists on the Telegram servers
   * (recommended) or pass “attach://<file_attach_name>” to upload a new one using multipart/form-data under
   * <file_attach_name> name. More information on Sending Files ». Sending live photos by a URL is currently
   * unsupported. */
  String media;
  /* The static photo to send. Pass a file_id to send a file that exists on the Telegram servers (recommended)
   * or pass “attach://<file_attach_name>” to upload a new one using multipart/form-data under
   * <file_attach_name> name. More information on Sending Files ». Sending live photos by a URL is currently
   * unsupported. */
  String photo;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("media", true).case_("photo", true).or_default(false);
  }
};

}  // namespace tgbm::api
