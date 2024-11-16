#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*This object represents one result of an inline query. Telegram clients currently support results of the
 * following 20 types:*/
struct InlineQueryResultLocation {
  /* Unique identifier for this result, 1-64 Bytes */
  String id;
  /* Location latitude in degrees */
  Double latitude;
  /* Location longitude in degrees */
  Double longitude;
  /* Location title */
  String title;
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
  /* Optional. Inline keyboard attached to the message */
  box<InlineKeyboardMarkup> reply_markup;
  /* Optional. Content of the message to be sent instead of the location */
  box<InputMessageContent> input_message_content;
  /* Optional. Url of the thumbnail for the result */
  optional<String> thumbnail_url;
  /* Optional. Thumbnail width */
  optional<Integer> thumbnail_width;
  /* Optional. Thumbnail height */
  optional<Integer> thumbnail_height;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name)
        .case_("id", true)
        .case_("latitude", true)
        .case_("longitude", true)
        .case_("title", true)
        .or_default(false);
  }
};

}  // namespace tgbm::api
