#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*Describes a checklist.*/
struct Checklist {
  /* Title of the checklist */
  String title;
  /* List of tasks in the checklist */
  arrayof<ChecklistTask> tasks;
  /* Optional. Special entities that appear in the checklist title */
  optional<arrayof<MessageEntity>> title_entities;
  /* Optional. True, if users other than the creator of the list can add tasks to the list */
  optional<True> others_can_add_tasks;
  /* Optional. True, if users other than the creator of the list can mark tasks as done or not done */
  optional<True> others_can_mark_tasks_as_done;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("title", true).case_("tasks", true).or_default(false);
  }
};

}  // namespace tgbm::api
