#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*This object represents a rich formatted text. Currently, it can be either a String for plain text, an Array
 * of RichText, or any of the following types:*/
struct RichTextDateTime {
  /* The text */
  box<RichText> text;
  /* The Unix time associated with the entity */
  Integer unix_time;
  /* The string that defines the formatting of the date and time. See date-time entity formatting for more
   * details. */
  String date_time_format;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name)
        .case_("text", true)
        .case_("unix_time", true)
        .case_("date_time_format", true)
        .or_default(false);
  }
};

}  // namespace tgbm::api
