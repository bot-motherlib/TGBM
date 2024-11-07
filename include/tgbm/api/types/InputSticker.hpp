#pragma once

#include "all_fwd.hpp"

namespace tgbm::api {

/*This object describes a sticker to be added to a sticker set.*/
struct InputSticker {
  /* The added sticker. Pass a file_id as a String to send a file that already exists on the Telegram servers,
   * pass an HTTP URL as a String for Telegram to get a file from the Internet, upload a new one using
   * multipart/form-data, or pass “attach://<file_attach_name>” to upload a new one using multipart/form-data
   * under <file_attach_name> name. Animated and video stickers can't be uploaded via HTTP URL. More
   * information on Sending Files » */
  file_or_str sticker;
  /* Format of the added sticker, must be one of “static” for a .WEBP or .PNG image, “animated” for a .TGS
   * animation, “video” for a WEBM video */
  String format;
  /* List of 1-20 emoji associated with the sticker */
  arrayof<String> emoji_list;
  /* Optional. Position where the mask should be placed on faces. For “mask” stickers only. */
  box<MaskPosition> mask_position;
  /* Optional. List of 0-20 search keywords for the sticker with total length of up to 64 characters. For
   * “regular” and “custom_emoji” stickers only. */
  optional<arrayof<String>> keywords;

  consteval static bool is_mandatory_field(std::string_view name) {
    return utils::string_switch<bool>(name)
        .case_("sticker", true)
        .case_("format", true)
        .case_("emoji_list", true)
        .or_default(false);
  }
};

}  // namespace tgbm::api
