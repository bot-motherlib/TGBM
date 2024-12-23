#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*This object contains information about the quoted part of a message that is replied to by the given
 * message.*/
struct TextQuote {
  /* Text of the quoted part of a message that is replied to by the given message */
  String text;
  /* Approximate quote position in the original message in UTF-16 code units as specified by the sender */
  Integer position;
  /* Optional. Special entities that appear in the quote. Currently, only bold, italic, underline,
   * strikethrough, spoiler, and custom_emoji entities are kept in quotes. */
  optional<arrayof<MessageEntity>> entities;
  /* Optional. True, if the quote was chosen manually by the message sender. Otherwise, the quote was added
   * automatically by the server. */
  optional<True> is_manual;

  constexpr static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("text", true).case_("position", true).or_default(false);
  }
};

}  // namespace tgbm::api
