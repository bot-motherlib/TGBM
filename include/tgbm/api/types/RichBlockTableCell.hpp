#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*Cell in a table.*/
struct RichBlockTableCell {
  /* Horizontal cell content alignment. Currently, must be one of “left”, “center”, or “right”. */
  String align;
  /* Vertical cell content alignment. Currently, must be one of “top”, “middle”, or “bottom”. */
  String valign;
  /* Optional. Text in the cell. If omitted, then the cell is invisible. */
  box<RichText> text;
  /* Optional. The number of columns the cell spans if it is bigger than 1 */
  optional<Integer> colspan;
  /* Optional. The number of rows the cell spans if it is bigger than 1 */
  optional<Integer> rowspan;
  /* Optional. True, if the cell is a header cell */
  optional<True> is_header;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("align", true).case_("valign", true).or_default(false);
  }
};

}  // namespace tgbm::api
