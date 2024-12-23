#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*This object represents a sticker set.*/
struct StickerSet {
  /* Sticker set name */
  String name;
  /* Sticker set title */
  String title;
  /* Type of stickers in the set, currently one of “regular”, “mask”, “custom_emoji” */
  String sticker_type;
  /* List of all set stickers */
  arrayof<Sticker> stickers;
  /* Optional. Sticker set thumbnail in the .WEBP, .TGS, or .WEBM format */
  box<PhotoSize> thumbnail;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name)
        .case_("name", true)
        .case_("title", true)
        .case_("sticker_type", true)
        .case_("stickers", true)
        .or_default(false);
  }
};

}  // namespace tgbm::api
