#pragma once

#include "tgbm/api/common.hpp"
#include "tgbm/api/types/Message.hpp"
#include "tgbm/api/types/ReplyParameters.hpp"

namespace tgbm::api {

struct send_location_request {
  /* Unique identifier for the target chat or username of the target channel (in the format @channelusername)
   */
  int_or_str chat_id;
  /* Latitude of the location */
  Double latitude;
  /* Longitude of the location */
  Double longitude;
  /* Unique identifier of the business connection on behalf of which the message will be sent */
  optional<String> business_connection_id;
  /* Unique identifier for the target message thread (topic) of the forum; for forum supergroups only */
  optional<Integer> message_thread_id;
  /* The radius of uncertainty for the location, measured in meters; 0-1500 */
  optional<Double> horizontal_accuracy;
  /* Period in seconds during which the location will be updated (see Live Locations, should be between 60 and
   * 86400, or 0x7FFFFFFF for live locations that can be edited indefinitely. */
  optional<Integer> live_period;
  /* For live locations, a direction in which the user is moving, in degrees. Must be between 1 and 360 if
   * specified. */
  optional<Integer> heading;
  /* For live locations, a maximum distance for proximity alerts about approaching another chat member, in
   * meters. Must be between 1 and 100000 if specified. */
  optional<Integer> proximity_alert_radius;
  /* Sends the message silently. Users will receive a notification with no sound. */
  optional<bool> disable_notification;
  /* Protects the contents of the sent message from forwarding and saving */
  optional<bool> protect_content;
  /* Pass True to allow up to 1000 messages per second, ignoring broadcasting limits for a fee of 0.1 Telegram
   * Stars per message. The relevant Stars will be withdrawn from the bot's balance */
  optional<bool> allow_paid_broadcast;
  /* Unique identifier of the message effect to be added to the message; for private chats only */
  optional<String> message_effect_id;
  /* Description of the message to reply to */
  box<ReplyParameters> reply_parameters;
  /* Additional interface options. A JSON-serialized object for an inline keyboard, custom reply keyboard,
   * instructions to remove a reply keyboard or to force a reply from the user */
  optional<reply_markup_t> reply_markup;

  using return_type = Message;
  static constexpr file_info_e file_info = file_info_e::no;
  static constexpr std::string_view api_method_name = "sendLocation";
  static constexpr http_method_e http_method = http_method_e::POST;

  void fill_nonfile_args(auto& body) const {
    if (business_connection_id)
      body.arg("business_connection_id", *business_connection_id);
    body.arg("chat_id", chat_id);
    if (message_thread_id)
      body.arg("message_thread_id", *message_thread_id);
    body.arg("latitude", latitude);
    body.arg("longitude", longitude);
    if (horizontal_accuracy)
      body.arg("horizontal_accuracy", *horizontal_accuracy);
    if (live_period)
      body.arg("live_period", *live_period);
    if (heading)
      body.arg("heading", *heading);
    if (proximity_alert_radius)
      body.arg("proximity_alert_radius", *proximity_alert_radius);
    if (disable_notification)
      body.arg("disable_notification", *disable_notification);
    if (protect_content)
      body.arg("protect_content", *protect_content);
    if (allow_paid_broadcast)
      body.arg("allow_paid_broadcast", *allow_paid_broadcast);
    if (message_effect_id)
      body.arg("message_effect_id", *message_effect_id);
    if (reply_parameters)
      body.arg("reply_parameters", *reply_parameters);
    if (reply_markup)
      body.arg("reply_markup", *reply_markup);
  }
};

}  // namespace tgbm::api
