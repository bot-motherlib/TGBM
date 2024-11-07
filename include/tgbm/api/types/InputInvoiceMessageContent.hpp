#pragma once

#include "all_fwd.hpp"

namespace tgbm::api {

/*This object represents the content of a message to be sent as a result of an inline query. Telegram clients
 * currently support the following 5 types:*/
struct InputInvoiceMessageContent {
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
  /* Optional. Payment provider token, obtained via @BotFather. Pass an empty string for payments in Telegram
   * Stars. */
  optional<String> provider_token;
  /* Optional. The maximum accepted amount for tips in the smallest units of the currency (integer, not
   * float/double). For example, for a maximum tip of US$ 1.45 pass max_tip_amount = 145. See the exp
   * parameter in currencies.json, it shows the number of digits past the decimal point for each currency (2
   * for the majority of currencies). Defaults to 0. Not supported for payments in Telegram Stars. */
  optional<Integer> max_tip_amount;
  /* Optional. A JSON-serialized array of suggested amounts of tip in the smallest units of the currency
   * (integer, not float/double). At most 4 suggested tip amounts can be specified. The suggested tip amounts
   * must be positive, passed in a strictly increased order and must not exceed max_tip_amount. */
  optional<arrayof<Integer>> suggested_tip_amounts;
  /* Optional. A JSON-serialized object for data about the invoice, which will be shared with the payment
   * provider. A detailed description of the required fields should be provided by the payment provider. */
  optional<String> provider_data;
  /* Optional. URL of the product photo for the invoice. Can be a photo of the goods or a marketing image for
   * a service. */
  optional<String> photo_url;
  /* Optional. Photo size in bytes */
  optional<Integer> photo_size;
  /* Optional. Photo width */
  optional<Integer> photo_width;
  /* Optional. Photo height */
  optional<Integer> photo_height;
  /* Optional. Pass True if you require the user's full name to complete the order. Ignored for payments in
   * Telegram Stars. */
  optional<bool> need_name;
  /* Optional. Pass True if you require the user's phone number to complete the order. Ignored for payments in
   * Telegram Stars. */
  optional<bool> need_phone_number;
  /* Optional. Pass True if you require the user's email address to complete the order. Ignored for payments
   * in Telegram Stars. */
  optional<bool> need_email;
  /* Optional. Pass True if you require the user's shipping address to complete the order. Ignored for
   * payments in Telegram Stars. */
  optional<bool> need_shipping_address;
  /* Optional. Pass True if the user's phone number should be sent to the provider. Ignored for payments in
   * Telegram Stars. */
  optional<bool> send_phone_number_to_provider;
  /* Optional. Pass True if the user's email address should be sent to the provider. Ignored for payments in
   * Telegram Stars. */
  optional<bool> send_email_to_provider;
  /* Optional. Pass True if the final price depends on the shipping method. Ignored for payments in Telegram
   * Stars. */
  optional<bool> is_flexible;

  consteval static bool is_mandatory_field(std::string_view name) {
    return utils::string_switch<bool>(name)
        .case_("description", true)
        .case_("payload", true)
        .case_("currency", true)
        .case_("prices", true)
        .or_default(false);
  }
};

}  // namespace tgbm::api
