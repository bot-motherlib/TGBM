#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*Describes a task in a checklist.*/
struct ChecklistTask {
  /* Unique identifier of the task */
  Integer id;
  /* Text of the task */
  String text;
  /* Optional. Special entities that appear in the task text */
  optional<arrayof<MessageEntity>> text_entities;
  /* Optional. User that completed the task; omitted if the task wasn't completed */
  box<User> completed_by_user;
  /* Optional. Point in time (Unix timestamp) when the task was completed; 0 if the task wasn't completed */
  optional<Integer> completion_date;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("id", true).case_("text", true).or_default(false);
  }
};

}  // namespace tgbm::api
