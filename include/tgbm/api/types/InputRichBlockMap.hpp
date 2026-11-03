#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*A block with a map, corresponding to the custom HTML tag <tg-map>. The map's width and height must not
 * exceed 10000 in total. The width and height ratio must be at most 20.*/
struct InputRichBlockMap {
  /* Location of the center of the map */
  box<Location> location;
  /* Optional. Map zoom level; 0-24 */
  optional<Integer> zoom;
  /* Optional. Map width; 0-10000 */
  optional<Integer> width;
  /* Optional. Map height; 0-10000 */
  optional<Integer> height;
  /* Optional. Caption of the block */
  box<RichBlockCaption> caption;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("location", true).or_default(false);
  }
};

}  // namespace tgbm::api
