#pragma once

#include "all_fwd.hpp"

namespace tgbm::api {

/*This object represents a sticker.*/
struct Sticker {
  /* Identifier for this file, which can be used to download or reuse the file */
  String file_id;
  /* Unique identifier for this file, which is supposed to be the same over time and for different bots. Can't
   * be used to download or reuse the file. */
  String file_unique_id;
  /* Type of the sticker, currently one of “regular”, “mask”, “custom_emoji”. The type of the sticker is
   * independent from its format, which is determined by the fields is_animated and is_video. */
  String type;
  /* Sticker width */
  Integer width;
  /* Sticker height */
  Integer height;
  /* True, if the sticker is animated */
  bool is_animated;
  /* True, if the sticker is a video sticker */
  bool is_video;
  /* Optional. Sticker thumbnail in the .WEBP or .JPG format */
  box<PhotoSize> thumbnail;
  /* Optional. Emoji associated with the sticker */
  optional<String> emoji;
  /* Optional. Name of the sticker set to which the sticker belongs */
  optional<String> set_name;
  /* Optional. For premium regular stickers, premium animation for the sticker */
  box<File> premium_animation;
  /* Optional. For mask stickers, the position where the mask should be placed */
  box<MaskPosition> mask_position;
  /* Optional. For custom emoji stickers, unique identifier of the custom emoji */
  optional<String> custom_emoji_id;
  /* Optional. File size in bytes */
  optional<Integer> file_size;
  /* Optional. True, if the sticker must be repainted to a text color in messages, the color of the Telegram
   * Premium badge in emoji status, white color on chat photos, or another appropriate color in other places
   */
  optional<True> needs_repainting;

  consteval static bool is_mandatory_field(std::string_view name) {
    return utils::string_switch<bool>(name)
        .case_("file_id", true)
        .case_("file_unique_id", true)
        .case_("type", true)
        .case_("width", true)
        .case_("height", true)
        .case_("is_animated", true)
        .case_("is_video", true)
        .or_default(false);
  }
};

}  // namespace tgbm::api
