#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*Describes a keyboard button to be used by a user of a Mini App.*/
struct PreparedKeyboardButton {
  /* Unique identifier of the keyboard button */
  String id;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("id", true).or_default(false);
  }
};

}  // namespace tgbm::api
