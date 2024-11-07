#pragma once

#include "all_fwd.hpp"

namespace tgbm::api {

/*This object represents an inline keyboard button that copies specified text to the clipboard.*/
struct CopyTextButton {
  /* The text to be copied to the clipboard; 1-256 characters */
  String text;

  consteval static bool is_mandatory_field(std::string_view name) {
    return utils::string_switch<bool>(name).case_("text", true).or_default(false);
  }
};

}  // namespace tgbm::api
