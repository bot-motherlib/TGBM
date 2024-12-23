#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*This object represents one special entity in a text message. For example, hashtags, usernames, URLs, etc.*/
struct MessageEntity {
  /* Type of the entity. Currently, can be “mention” (@username), “hashtag” (#hashtag or
   * #hashtag@chatusername), “cashtag” ($USD or $USD@chatusername), “bot_command” (/start@jobs_bot), “url”
   * (https://telegram.org), “email” (do-not-reply@telegram.org), “phone_number” (+1-212-555-0123), “bold”
   * (bold text), “italic” (italic text), “underline” (underlined text), “strikethrough” (strikethrough text),
   * “spoiler” (spoiler message), “blockquote” (block quotation), “expandable_blockquote”
   * (collapsed-by-default block quotation), “code” (monowidth string), “pre” (monowidth block), “text_link”
   * (for clickable text URLs), “text_mention” (for users without usernames), “custom_emoji” (for inline
   * custom emoji stickers) */
  String type;
  /* Offset in UTF-16 code units to the start of the entity */
  Integer offset;
  /* Length of the entity in UTF-16 code units */
  Integer length;
  /* Optional. For “text_link” only, URL that will be opened after user taps on the text */
  optional<String> url;
  /* Optional. For “text_mention” only, the mentioned user */
  box<User> user;
  /* Optional. For “pre” only, the programming language of the entity text */
  optional<String> language;
  /* Optional. For “custom_emoji” only, unique identifier of the custom emoji. Use getCustomEmojiStickers to
   * get full information about the sticker */
  optional<String> custom_emoji_id;

  constexpr static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name)
        .case_("type", true)
        .case_("offset", true)
        .case_("length", true)
        .or_default(false);
  }
};

}  // namespace tgbm::api
