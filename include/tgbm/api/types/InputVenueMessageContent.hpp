#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*This object represents the content of a message to be sent as a result of an inline query. Telegram clients
 * currently support the following 5 types:*/
struct InputVenueMessageContent {
  /* Longitude of the venue in degrees */
  Double longitude;
  /* Name of the venue */
  String title;
  /* Address of the venue */
  String address;
  /* Optional. Foursquare identifier of the venue, if known */
  optional<String> foursquare_id;
  /* Optional. Foursquare type of the venue, if known. (For example, “arts_entertainment/default”,
   * “arts_entertainment/aquarium” or “food/icecream”.) */
  optional<String> foursquare_type;
  /* Optional. Google Places identifier of the venue */
  optional<String> google_place_id;
  /* Optional. Google Places type of the venue. (See supported types.) */
  optional<String> google_place_type;

  constexpr static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name)
        .case_("longitude", true)
        .case_("title", true)
        .case_("address", true)
        .or_default(false);
  }
};

}  // namespace tgbm::api
