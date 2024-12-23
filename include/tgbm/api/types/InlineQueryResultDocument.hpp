#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*This object represents one result of an inline query. Telegram clients currently support results of the
 * following 20 types:*/
struct InlineQueryResultDocument {
  /* Unique identifier for this result, 1-64 bytes */
  String id;
  /* Title for the result */
  String title;
  /* A valid URL for the file */
  String document_url;
  /* MIME type of the content of the file, either “application/pdf” or “application/zip” */
  String mime_type;
  /* Optional. Caption of the document to be sent, 0-1024 characters after entities parsing */
  optional<String> caption;
  /* Optional. Mode for parsing entities in the document caption. See formatting options for more details. */
  optional<String> parse_mode;
  /* Optional. List of special entities that appear in the caption, which can be specified instead of
   * parse_mode */
  optional<arrayof<MessageEntity>> caption_entities;
  /* Optional. Short description of the result */
  optional<String> description;
  /* Optional. Inline keyboard attached to the message */
  box<InlineKeyboardMarkup> reply_markup;
  /* Optional. Content of the message to be sent instead of the file */
  box<InputMessageContent> input_message_content;
  /* Optional. URL of the thumbnail (JPEG only) for the file */
  optional<String> thumbnail_url;
  /* Optional. Thumbnail width */
  optional<Integer> thumbnail_width;
  /* Optional. Thumbnail height */
  optional<Integer> thumbnail_height;

  constexpr static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name)
        .case_("id", true)
        .case_("title", true)
        .case_("document_url", true)
        .case_("mime_type", true)
        .or_default(false);
  }
};

}  // namespace tgbm::api
