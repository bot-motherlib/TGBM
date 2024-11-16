#pragma once

#include <tgbm/api/common.hpp>
#include <tgbm/api/types/InlineKeyboardMarkup.hpp>
#include <tgbm/api/types/Message.hpp>

namespace tgbm::api {

struct edit_message_live_location_request {
  /* Latitude of new location */
  Double latitude;
  /* Longitude of new location */
  Double longitude;
  /* Unique identifier of the business connection on behalf of which the message to be edited was sent */
  optional<String> business_connection_id;
  /* Required if inline_message_id is not specified. Unique identifier for the target chat or username of the
   * target channel (in the format @channelusername) */
  optional<int_or_str> chat_id;
  /* Required if inline_message_id is not specified. Identifier of the message to edit */
  optional<Integer> message_id;
  /* Required if chat_id and message_id are not specified. Identifier of the inline message */
  optional<String> inline_message_id;
  /* New period in seconds during which the location can be updated, starting from the message send date. If
   * 0x7FFFFFFF is specified, then the location can be updated forever. Otherwise, the new value must not
   * exceed the current live_period by more than a day, and the live location expiration date must remain
   * within the next 90 days. If not specified, then live_period remains unchanged */
  optional<Integer> live_period;
  /* The radius of uncertainty for the location, measured in meters; 0-1500 */
  optional<Double> horizontal_accuracy;
  /* Direction in which the user is moving, in degrees. Must be between 1 and 360 if specified. */
  optional<Integer> heading;
  /* The maximum distance for proximity alerts about approaching another chat member, in meters. Must be
   * between 1 and 100000 if specified. */
  optional<Integer> proximity_alert_radius;
  /* A JSON-serialized object for a new inline keyboard. */
  box<InlineKeyboardMarkup> reply_markup;

  using return_type = oneof<bool, Message>;
  static constexpr file_info_e file_info = file_info_e::no;
  static constexpr std::string_view api_method_name = "editMessageLiveLocation";
  static constexpr http_method_e http_method = http_method_e::POST;

  void fill_nonfile_args(auto& body) const {
    if (business_connection_id)
      body.arg("business_connection_id", *business_connection_id);
    if (chat_id)
      body.arg("chat_id", *chat_id);
    if (message_id)
      body.arg("message_id", *message_id);
    if (inline_message_id)
      body.arg("inline_message_id", *inline_message_id);
    body.arg("latitude", latitude);
    body.arg("longitude", longitude);
    if (live_period)
      body.arg("live_period", *live_period);
    if (horizontal_accuracy)
      body.arg("horizontal_accuracy", *horizontal_accuracy);
    if (heading)
      body.arg("heading", *heading);
    if (proximity_alert_radius)
      body.arg("proximity_alert_radius", *proximity_alert_radius);
    if (reply_markup)
      body.arg("reply_markup", *reply_markup);
  }
};

}  // namespace tgbm::api
