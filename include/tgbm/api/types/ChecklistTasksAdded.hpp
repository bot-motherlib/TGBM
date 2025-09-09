#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*Describes a service message about tasks added to a checklist.*/
struct ChecklistTasksAdded {
  /* List of tasks added to the checklist */
  arrayof<ChecklistTask> tasks;
  /* Optional. Message containing the checklist to which the tasks were added. Note that the Message object in
   * this field will not contain the reply_to_message field even if it itself is a reply. */
  box<Message> checklist_message;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("tasks", true).or_default(false);
  }
};

}  // namespace tgbm::api
