#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*An item of a list to be sent.*/
struct InputRichBlockListItem {
  /* The content of the item */
  arrayof<InputRichBlock> blocks;
  /* Optional. For ordered lists, the numeric value of the item label */
  optional<Integer> value;
  /* Optional. For ordered lists, the type of the item label; must be one of “a” for lowercase letters, “A”
   * for uppercase letters, “i” for lowercase Roman numerals, “I” for uppercase Roman numerals, or “1” for
   * decimal numbers */
  optional<String> type;
  /* Optional. Pass True if the item has a checkbox */
  optional<True> has_checkbox;
  /* Optional. Pass True if the item has a checked checkbox */
  optional<True> is_checked;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("blocks", true).or_default(false);
  }
};

}  // namespace tgbm::api
