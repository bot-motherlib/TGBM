#pragma once

#include "all_fwd.hpp"

namespace tgbm::api {

/*This object describes the paid media to be sent. Currently, it can be one of*/
struct InputPaidMediaPhoto {
  /* File to send. Pass a file_id to send a file that exists on the Telegram servers (recommended), pass an
   * HTTP URL for Telegram to get a file from the Internet, or pass “attach://<file_attach_name>” to upload a
   * new one using multipart/form-data under <file_attach_name> name. More information on Sending Files » */
  String media;

  consteval static bool is_mandatory_field(std::string_view name) {
    return utils::string_switch<bool>(name).case_("media", true).or_default(false);
  }
};

}  // namespace tgbm::api
