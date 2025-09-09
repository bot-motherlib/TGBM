#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*Describes a service message about checklist tasks marked as done or not done.*/
struct ChecklistTasksDone {
  /* Optional. Message containing the checklist whose tasks were marked as done or not done. Note that the
   * Message object in this field will not contain the reply_to_message field even if it itself is a reply. */
  box<Message> checklist_message;
  /* Optional. Identifiers of the tasks that were marked as done */
  optional<arrayof<Integer>> marked_as_done_task_ids;
  /* Optional. Identifiers of the tasks that were marked as not done */
  optional<arrayof<Integer>> marked_as_not_done_task_ids;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).or_default(false);
  }
};

}  // namespace tgbm::api
