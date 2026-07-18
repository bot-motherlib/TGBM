#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*An item of a list.*/
struct RichBlockListItem {
  /* Label of the item */
  String label;
  /* The content of the item */
  arrayof<RichBlock> blocks;
  /* Optional. For ordered lists, the numeric value of the item label */
  optional<Integer> value;
  /* Optional. For ordered lists, the type of the item label; must be one of “a” for lowercase letters, “A”
   * for uppercase letters, “i” for lowercase Roman numerals, “I” for uppercase Roman numerals, or “1” for
   * decimal numbers */
  optional<String> type;
  /* Optional. True, if the item has a checkbox */
  optional<True> has_checkbox;
  /* Optional. True, if the item has a checked checkbox */
  optional<True> is_checked;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("label", true).case_("blocks", true).or_default(false);
  }
};

}  // namespace tgbm::api
