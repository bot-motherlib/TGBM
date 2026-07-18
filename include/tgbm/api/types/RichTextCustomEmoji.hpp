#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*This object represents a rich formatted text. Currently, it can be either a String for plain text, an Array
 * of RichText, or any of the following types:*/
struct RichTextCustomEmoji {
  /* Unique identifier of the custom emoji. Use getCustomEmojiStickers to get full information about the
   * sticker. */
  String custom_emoji_id;
  /* Alternative emoji for the custom emoji */
  String alternative_text;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name)
        .case_("custom_emoji_id", true)
        .case_("alternative_text", true)
        .or_default(false);
  }
};

}  // namespace tgbm::api
