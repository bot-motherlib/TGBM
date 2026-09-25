#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*A mention by a username.*/
struct RichTextMention {
  /* The text */
  box<RichText> text;
  /* The username */
  String username;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("text", true).case_("username", true).or_default(false);
  }
};

}  // namespace tgbm::api
