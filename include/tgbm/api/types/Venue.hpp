#pragma once

#include "all_fwd.hpp"

namespace tgbm::api {

/*This object represents a venue.*/
struct Venue {
  /* Venue location. Can't be a live location */
  box<Location> location;
  /* Name of the venue */
  String title;
  /* Address of the venue */
  String address;
  /* Optional. Foursquare identifier of the venue */
  optional<String> foursquare_id;
  /* Optional. Foursquare type of the venue. (For example, “arts_entertainment/default”,
   * “arts_entertainment/aquarium” or “food/icecream”.) */
  optional<String> foursquare_type;
  /* Optional. Google Places identifier of the venue */
  optional<String> google_place_id;
  /* Optional. Google Places type of the venue. (See supported types.) */
  optional<String> google_place_type;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name)
        .case_("location", true)
        .case_("title", true)
        .case_("address", true)
        .or_default(false);
  }
};

}  // namespace tgbm::api
