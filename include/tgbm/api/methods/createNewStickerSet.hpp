#pragma once

#include <tgbm/api/common.hpp>
#include <tgbm/api/types/InputSticker.hpp>

namespace tgbm::api {

struct create_new_sticker_set_request {
  /* User identifier of created sticker set owner */
  Integer user_id;
  /* Short name of sticker set, to be used in t.me/addstickers/ URLs (e.g., animals). Can contain only English
   * letters, digits and underscores. Must begin with a letter, can't contain consecutive underscores and must
   * end in "_by_<bot_username>". <bot_username> is case insensitive. 1-64 characters. */
  String name;
  /* Sticker set title, 1-64 characters */
  String title;
  /* A JSON-serialized list of 1-50 initial stickers to be added to the sticker set */
  arrayof<InputSticker> stickers;
  /* Type of stickers in the set, pass “regular”, “mask”, or “custom_emoji”. By default, a regular sticker set
   * is created. */
  optional<String> sticker_type;
  /* Pass True if stickers in the sticker set must be repainted to the color of text when used in messages,
   * the accent color if used as emoji status, white on chat photos, or another appropriate color based on
   * context; for custom emoji sticker sets only */
  optional<bool> needs_repainting;

  using return_type = bool;
  static constexpr file_info_e file_info = file_info_e::no;
  static constexpr std::string_view api_method_name = "createNewStickerSet";
  static constexpr http_method_e http_method = http_method_e::POST;

  void fill_nonfile_args(auto& body) const {
    body.arg("user_id", user_id);
    body.arg("name", name);
    body.arg("title", title);
    body.arg("stickers", stickers);
    if (sticker_type)
      body.arg("sticker_type", *sticker_type);
    if (needs_repainting)
      body.arg("needs_repainting", *needs_repainting);
  }
};

}  // namespace tgbm::api
