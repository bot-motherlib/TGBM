#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*A table, corresponding to the HTML tag <table>.*/
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
