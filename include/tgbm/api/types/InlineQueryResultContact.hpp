#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*Represents a contact with a phone number. By default, this contact will be sent by the user. Alternatively,
 * you can use input_message_content to send a message with the specified content instead of the contact.*/
struct InlineQueryResultContact {
  /* Unique identifier for this result, 1-64 Bytes */
  String id;
  /* Contact's phone number */
  String phone_number;
  /* Contact's first name */
  String first_name;
  /* Optional. Contact's last name */
  optional<String> last_name;
  /* Optional. Additional data about the contact in the form of a vCard, 0-2048 bytes */
  optional<String> vcard;
  /* Optional. Inline keyboard attached to the message */
  box<InlineKeyboardMarkup> reply_markup;
  /* Optional. Content of the message to be sent instead of the contact */
  box<InputMessageContent> input_message_content;
  /* Optional. Url of the thumbnail for the result */
  optional<String> thumbnail_url;
  /* Optional. Thumbnail width */
  optional<Integer> thumbnail_width;
  /* Optional. Thumbnail height */
  optional<Integer> thumbnail_height;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name)
        .case_("id", true)
        .case_("phone_number", true)
        .case_("first_name", true)
        .or_default(false);
  }
};

}  // namespace tgbm::api
