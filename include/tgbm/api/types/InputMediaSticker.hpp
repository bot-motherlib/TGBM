#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*Represents a sticker file to be sent.*/
struct InputMediaSticker {
  /* File to send. Pass a file_id to send a file that exists on the Telegram servers (recommended), pass an
   * HTTP URL for Telegram to get a .WEBP sticker from the Internet, or pass “attach://<file_attach_name>” to
   * upload a new .WEBP, .TGS, or .WEBM sticker using multipart/form-data under <file_attach_name> name. More
   * information on Sending Files » */
  String media;
  /* Optional. Emoji associated with the sticker; only for just uploaded stickers */
  optional<String> emoji;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("media", true).or_default(false);
  }
};

}  // namespace tgbm::api
