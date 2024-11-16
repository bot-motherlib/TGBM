#pragma once

#include "all_fwd.hpp"

namespace tgbm::api {

/*This object represents one result of an inline query. Telegram clients currently support results of the
 * following 20 types:*/
struct InlineQueryResultCachedSticker {
  /* Unique identifier for this result, 1-64 bytes */
  String id;
  /* A valid file identifier of the sticker */
  String sticker_file_id;
  /* Optional. Inline keyboard attached to the message */
  box<InlineKeyboardMarkup> reply_markup;
  /* Optional. Content of the message to be sent instead of the sticker */
  box<InputMessageContent> input_message_content;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("id", true).case_("sticker_file_id", true).or_default(false);
  }
};

}  // namespace tgbm::api
