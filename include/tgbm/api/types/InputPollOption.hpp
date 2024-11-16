#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*This object contains information about one answer option in a poll to be sent.*/
struct InputPollOption {
  /* Option text, 1-100 characters */
  String text;
  /* Optional. Mode for parsing entities in the text. See formatting options for more details. Currently, only
   * custom emoji entities are allowed */
  optional<String> text_parse_mode;
  /* Optional. A JSON-serialized list of special entities that appear in the poll option text. It can be
   * specified instead of text_parse_mode */
  optional<arrayof<MessageEntity>> text_entities;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("text", true).or_default(false);
  }
};

}  // namespace tgbm::api
