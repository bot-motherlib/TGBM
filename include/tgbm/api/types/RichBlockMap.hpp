#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*A block with a map, corresponding to the custom HTML tag <tg-map>.*/
struct RichBlockMap {
  /* Location of the center of the map */
  box<Location> location;
  /* Map zoom level; 13-20 */
  Integer zoom;
  /* Expected width of the map */
  Integer width;
  /* Expected height of the map */
  Integer height;
  /* Optional. Caption of the block */
  box<RichBlockCaption> caption;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name)
        .case_("location", true)
        .case_("zoom", true)
        .case_("width", true)
        .case_("height", true)
        .or_default(false);
  }
};

}  // namespace tgbm::api
