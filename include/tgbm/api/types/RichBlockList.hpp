#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*This object represents a block in a rich formatted message. Currently, it can be any of the following
 * types:*/
struct RichBlockList {
  /* Items of the list */
  arrayof<RichBlockListItem> items;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("items", true).or_default(false);
  }
};

}  // namespace tgbm::api
