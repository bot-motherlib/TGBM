#pragma once

#include <tgbm/api/common.hpp>
#include <tgbm/api/types/InlineKeyboardMarkup.hpp>
#include <tgbm/api/types/LabeledPrice.hpp>
#include <tgbm/api/types/Message.hpp>
#include <tgbm/api/types/ReplyParameters.hpp>

namespace tgbm::api {

struct send_invoice_request {
  /* Unique identifier for the target chat or username of the target channel (in the format @channelusername)
   */
  int_or_str chat_id;
  /* Product name, 1-32 characters */
  String title;
  /* Product description, 1-255 characters */
  String description;
  /* Bot-defined invoice payload, 1-128 bytes. This will not be displayed to the user, use it for your
   * internal processes. */
  String payload;
  /* Three-letter ISO 4217 currency code, see more on currencies. Pass “XTR” for payments in Telegram Stars.
   */
  String currency;
  /* Price breakdown, a JSON-serialized list of components (e.g. product price, tax, discount, delivery cost,
   * delivery tax, bonus, etc.). Must contain exactly one item for payments in Telegram Stars. */
  arrayof<LabeledPrice> prices;
  /* Unique identifier for the target message thread (topic) of the forum; for forum supergroups only */
  optional<Integer> message_thread_id;
  /* Payment provider token, obtained via @BotFather. Pass an empty string for payments in Telegram Stars. */
  optional<String> provider_token;
  /* The maximum accepted amount for tips in the smallest units of the currency (integer, not float/double).
   * For example, for a maximum tip of US$ 1.45 pass max_tip_amount = 145. See the exp parameter in
   * currencies.json, it shows the number of digits past the decimal point for each currency (2 for the
   * majority of currencies). Defaults to 0. Not supported for payments in Telegram Stars. */
  optional<Integer> max_tip_amount;
  /* A JSON-serialized array of suggested amounts of tips in the smallest units of the currency (integer, not
   * float/double). At most 4 suggested tip amounts can be specified. The suggested tip amounts must be
   * positive, passed in a strictly increased order and must not exceed max_tip_amount. */
  optional<arrayof<Integer>> suggested_tip_amounts;
  /* Unique deep-linking parameter. If left empty, forwarded copies of the sent message will have a Pay
   * button, allowing multiple users to pay directly from the forwarded message, using the same invoice. If
   * non-empty, forwarded copies of the sent message will have a URL button with a deep link to the bot
   * (instead of a Pay button), with the value used as the start parameter */
  optional<String> start_parameter;
  /* JSON-serialized data about the invoice, which will be shared with the payment provider. A detailed
   * description of required fields should be provided by the payment provider. */
  optional<String> provider_data;
  /* URL of the product photo for the invoice. Can be a photo of the goods or a marketing image for a service.
   * People like it better when they see what they are paying for. */
  optional<String> photo_url;
  /* Photo size in bytes */
  optional<Integer> photo_size;
  /* Photo width */
  optional<Integer> photo_width;
  /* Photo height */
  optional<Integer> photo_height;
  /* Pass True if you require the user's full name to complete the order. Ignored for payments in Telegram
   * Stars. */
  optional<bool> need_name;
  /* Pass True if you require the user's phone number to complete the order. Ignored for payments in Telegram
   * Stars. */
  optional<bool> need_phone_number;
  /* Pass True if you require the user's email address to complete the order. Ignored for payments in Telegram
   * Stars. */
  optional<bool> need_email;
  /* Pass True if you require the user's shipping address to complete the order. Ignored for payments in
   * Telegram Stars. */
  optional<bool> need_shipping_address;
  /* Pass True if the user's phone number should be sent to the provider. Ignored for payments in Telegram
   * Stars. */
  optional<bool> send_phone_number_to_provider;
  /* Pass True if the user's email address should be sent to the provider. Ignored for payments in Telegram
   * Stars. */
  optional<bool> send_email_to_provider;
  /* Pass True if the final price depends on the shipping method. Ignored for payments in Telegram Stars. */
  optional<bool> is_flexible;
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
  /* A JSON-serialized object for an inline keyboard. If empty, one 'Pay total price' button will be shown. If
   * not empty, the first button must be a Pay button. */
  box<InlineKeyboardMarkup> reply_markup;

  using return_type = Message;
  static constexpr file_info_e file_info = file_info_e::no;
  static constexpr std::string_view api_method_name = "sendInvoice";
  static constexpr http_method_e http_method = http_method_e::POST;

  void fill_nonfile_args(auto& body) const {
    body.arg("chat_id", chat_id);
    if (message_thread_id)
      body.arg("message_thread_id", *message_thread_id);
    body.arg("title", title);
    body.arg("description", description);
    body.arg("payload", payload);
    if (provider_token)
      body.arg("provider_token", *provider_token);
    body.arg("currency", currency);
    body.arg("prices", prices);
    if (max_tip_amount)
      body.arg("max_tip_amount", *max_tip_amount);
    if (suggested_tip_amounts)
      body.arg("suggested_tip_amounts", *suggested_tip_amounts);
    if (start_parameter)
      body.arg("start_parameter", *start_parameter);
    if (provider_data)
      body.arg("provider_data", *provider_data);
    if (photo_url)
      body.arg("photo_url", *photo_url);
    if (photo_size)
      body.arg("photo_size", *photo_size);
    if (photo_width)
      body.arg("photo_width", *photo_width);
    if (photo_height)
      body.arg("photo_height", *photo_height);
    if (need_name)
      body.arg("need_name", *need_name);
    if (need_phone_number)
      body.arg("need_phone_number", *need_phone_number);
    if (need_email)
      body.arg("need_email", *need_email);
    if (need_shipping_address)
      body.arg("need_shipping_address", *need_shipping_address);
    if (send_phone_number_to_provider)
      body.arg("send_phone_number_to_provider", *send_phone_number_to_provider);
    if (send_email_to_provider)
      body.arg("send_email_to_provider", *send_email_to_provider);
    if (is_flexible)
      body.arg("is_flexible", *is_flexible);
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
