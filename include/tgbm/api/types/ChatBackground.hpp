#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*This object represents a chat background.*/
struct ChatBackground {
  /* Type of the background */
  box<BackgroundType> type;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("type", true).or_default(false);
  }
};

}  // namespace tgbm::api
