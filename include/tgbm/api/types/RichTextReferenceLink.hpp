#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*A link to a reference.*/
struct RichTextReferenceLink {
  /* The link text */
  box<RichText> text;
  /* The name of the reference */
  String reference_name;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("text", true).case_("reference_name", true).or_default(false);
  }
};

}  // namespace tgbm::api
