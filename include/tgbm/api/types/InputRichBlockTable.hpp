#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*This object represents a block in a rich formatted message to be sent. Currently, it can be any of the
 * following types:*/
struct InputRichBlockTable {
  /* Cells of the table */
  arrayof<arrayof<RichBlockTableCell>> cells;
  /* Optional. Caption of the table */
  box<RichText> caption;
  /* Optional. Pass True if the table has borders */
  optional<True> is_bordered;
  /* Optional. Pass True if the table is striped */
  optional<True> is_striped;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("cells", true).or_default(false);
  }
};

}  // namespace tgbm::api
