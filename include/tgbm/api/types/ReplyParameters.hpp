#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*Describes reply parameters for the message that is being sent.*/
struct ReplyParameters {
  /* Identifier of the message that will be replied to in the current chat, or in the chat chat_id if it is
   * specified */
  Integer message_id;
  /* Optional. If the message to be replied to is from a different chat, unique identifier for the chat or
   * username of the channel (in the format @channelusername). Not supported for messages sent on behalf of a
   * business account. */
  optional<int_or_str> chat_id;
  /* Optional. Quoted part of the message to be replied to; 0-1024 characters after entities parsing. The
   * quote must be an exact substring of the message to be replied to, including bold, italic, underline,
   * strikethrough, spoiler, and custom_emoji entities. The message will fail to send if the quote isn't found
   * in the original message. */
  optional<String> quote;
  /* Optional. Mode for parsing entities in the quote. See formatting options for more details. */
  optional<String> quote_parse_mode;
  /* Optional. A JSON-serialized list of special entities that appear in the quote. It can be specified
   * instead of quote_parse_mode. */
  optional<arrayof<MessageEntity>> quote_entities;
  /* Optional. Position of the quote in the original message in UTF-16 code units */
  optional<Integer> quote_position;
  /* Optional. Pass True if the message should be sent even if the specified message to be replied to is not
   * found. Always False for replies in another chat or forum topic. Always True for messages sent on behalf
   * of a business account. */
  optional<bool> allow_sending_without_reply;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("message_id", true).or_default(false);
  }
};

}  // namespace tgbm::api
