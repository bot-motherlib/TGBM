#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*Describes a clickable area on a story media.*/
struct StoryArea {
  /* Position of the area */
  box<StoryAreaPosition> position;
  /* Type of the area */
  box<StoryAreaType> type;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("position", true).case_("type", true).or_default(false);
  }
};

}  // namespace tgbm::api
