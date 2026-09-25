#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*A list of blocks, corresponding to the HTML tag <ul> or <ol> with multiple nested tags <li>.*/
struct RichBlockList {
  /* Items of the list */
  arrayof<RichBlockListItem> items;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("items", true).or_default(false);
  }
};

}  // namespace tgbm::api
