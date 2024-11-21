#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*This object represents an inline keyboard that appears right next to the message it belongs to.*/
struct InlineKeyboardMarkup {
  /* Array of button rows, each represented by an Array of InlineKeyboardButton objects */
  arrayof<arrayof<InlineKeyboardButton>> inline_keyboard;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("inline_keyboard", true).or_default(false);
  }
};

}  // namespace tgbm::api
