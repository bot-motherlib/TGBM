#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*This object represents a rich formatted text. Currently, it can be either a String for plain text, an Array
 * of RichText, or any of the following types:*/
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
