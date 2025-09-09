#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*Describes a task to add to a checklist.*/
struct InputChecklistTask {
  /* Unique identifier of the task; must be positive and unique among all task identifiers currently present
   * in the checklist */
  Integer id;
  /* Text of the task; 1-100 characters after entities parsing */
  String text;
  /* Optional. Mode for parsing entities in the text. See formatting options for more details. */
  optional<String> parse_mode;
  /* Optional. List of special entities that appear in the text, which can be specified instead of parse_mode.
   * Currently, only bold, italic, underline, strikethrough, spoiler, and custom_emoji entities are allowed.
   */
  optional<arrayof<MessageEntity>> text_entities;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("id", true).case_("text", true).or_default(false);
  }
};

}  // namespace tgbm::api
