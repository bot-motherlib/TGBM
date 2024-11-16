#pragma once

#include "all_fwd.hpp"

namespace tgbm::api {

/*This object represents one result of an inline query. Telegram clients currently support results of the
 * following 20 types:*/
struct InlineQueryResultVenue {
  /* Unique identifier for this result, 1-64 Bytes */
  String id;
  /* Latitude of the venue location in degrees */
  Double latitude;
  /* Longitude of the venue location in degrees */
  Double longitude;
  /* Title of the venue */
  String title;
  /* Address of the venue */
  String address;
  /* Optional. Foursquare identifier of the venue if known */
  optional<String> foursquare_id;
  /* Optional. Foursquare type of the venue, if known. (For example, “arts_entertainment/default”,
   * “arts_entertainment/aquarium” or “food/icecream”.) */
  optional<String> foursquare_type;
  /* Optional. Google Places identifier of the venue */
  optional<String> google_place_id;
  /* Optional. Google Places type of the venue. (See supported types.) */
  optional<String> google_place_type;
  /* Optional. Inline keyboard attached to the message */
  box<InlineKeyboardMarkup> reply_markup;
  /* Optional. Content of the message to be sent instead of the venue */
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
        .case_("address", true)
        .or_default(false);
  }
};

}  // namespace tgbm::api
