#pragma once

#include "all_fwd.hpp"

namespace tgbm::api {

/*This object represents a point on the map.*/
struct Location {
  /* Latitude as defined by the sender */
  Double latitude;
  /* Longitude as defined by the sender */
  Double longitude;
  /* Optional. The radius of uncertainty for the location, measured in meters; 0-1500 */
  optional<Double> horizontal_accuracy;
  /* Optional. Time relative to the message sending date, during which the location can be updated; in
   * seconds. For active live locations only. */
  optional<Integer> live_period;
  /* Optional. The direction in which user is moving, in degrees; 1-360. For active live locations only. */
  optional<Integer> heading;
  /* Optional. The maximum distance for proximity alerts about approaching another chat member, in meters. For
   * sent live locations only. */
  optional<Integer> proximity_alert_radius;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("latitude", true).case_("longitude", true).or_default(false);
  }
};

}  // namespace tgbm::api
