#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*A cashtag.*/
struct RichTextCashtag {
  /* The text */
  box<RichText> text;
  /* The cashtag */
  String cashtag;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("text", true).case_("cashtag", true).or_default(false);
  }
};

}  // namespace tgbm::api
