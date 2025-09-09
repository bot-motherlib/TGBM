#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*Describes a checklist to create.*/
struct InputChecklist {
  /* Title of the checklist; 1-255 characters after entities parsing */
  String title;
  /* List of 1-30 tasks in the checklist */
  arrayof<InputChecklistTask> tasks;
  /* Optional. Mode for parsing entities in the title. See formatting options for more details. */
  optional<String> parse_mode;
  /* Optional. List of special entities that appear in the title, which can be specified instead of
   * parse_mode. Currently, only bold, italic, underline, strikethrough, spoiler, and custom_emoji entities
   * are allowed. */
  optional<arrayof<MessageEntity>> title_entities;
  /* Optional. Pass True if other users can add tasks to the checklist */
  optional<bool> others_can_add_tasks;
  /* Optional. Pass True if other users can mark tasks as done or not done in the checklist */
  optional<bool> others_can_mark_tasks_as_done;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("title", true).case_("tasks", true).or_default(false);
  }
};

}  // namespace tgbm::api
