#pragma once

#include "tgbm/api/common.hpp"

namespace tgbm::api {

struct set_custom_emoji_sticker_set_thumbnail_request {
  /* Sticker set name */
  String name;
  /* Custom emoji identifier of a sticker from the sticker set; pass an empty string to drop the thumbnail and
   * use the first sticker as the thumbnail. */
  optional<String> custom_emoji_id;

  using return_type = bool;
  static constexpr file_info_e file_info = file_info_e::no;
  static constexpr std::string_view api_method_name = "setCustomEmojiStickerSetThumbnail";
  static constexpr http_method_e http_method = http_method_e::POST;

  void fill_nonfile_args(auto& body) const {
    body.arg("name", name);
    if (custom_emoji_id)
      body.arg("custom_emoji_id", *custom_emoji_id);
  }
};

}  // namespace tgbm::api
