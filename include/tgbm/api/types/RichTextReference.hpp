#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*A reference.*/
struct RichTextReference {
  /* Text of the reference */
  box<RichText> text;
  /* The name of the reference */
  String name;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("text", true).case_("name", true).or_default(false);
  }
};

}  // namespace tgbm::api
