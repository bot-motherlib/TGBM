#pragma once

#include "all_fwd.hpp"

namespace tgbm::api {

/*This object represents the content of a message to be sent as a result of an inline query. Telegram clients
 * currently support the following 5 types:*/
struct InputLocationMessageContent {
  /* Longitude of the location in degrees */
  Double longitude;
  /* Optional. The radius of uncertainty for the location, measured in meters; 0-1500 */
  optional<Double> horizontal_accuracy;
  /* Optional. Period in seconds during which the location can be updated, should be between 60 and 86400, or
   * 0x7FFFFFFF for live locations that can be edited indefinitely. */
  optional<Integer> live_period;
  /* Optional. For live locations, a direction in which the user is moving, in degrees. Must be between 1 and
   * 360 if specified. */
  optional<Integer> heading;
  /* Optional. For live locations, a maximum distance for proximity alerts about approaching another chat
   * member, in meters. Must be between 1 and 100000 if specified. */
  optional<Integer> proximity_alert_radius;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("longitude", true).or_default(false);
  }
};

}  // namespace tgbm::api
