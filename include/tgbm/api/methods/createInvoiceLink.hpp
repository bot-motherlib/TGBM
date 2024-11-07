#pragma once

#include "tgbm/api/common.hpp"
#include "tgbm/api/types/LabeledPrice.hpp"

namespace tgbm::api {

struct create_invoice_link_request {
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
  /* JSON-serialized data about the invoice, which will be shared with the payment provider. A detailed
   * description of required fields should be provided by the payment provider. */
  optional<String> provider_data;
  /* URL of the product photo for the invoice. Can be a photo of the goods or a marketing image for a service.
   */
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

  using return_type = String;
  static constexpr file_info_e file_info = file_info_e::no;
  static constexpr std::string_view api_method_name = "createInvoiceLink";
  static constexpr http_method_e http_method = http_method_e::POST;

  void fill_nonfile_args(auto& body) const {
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
  }
};

}  // namespace tgbm::api
