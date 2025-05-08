#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*This object describes the source of a transaction, or its recipient for outgoing transactions. Currently, it
 * can be one of*/
struct TransactionPartnerUser {
  /* Type of the transaction, currently one of “invoice_payment” for payments via invoices,
   * “paid_media_payment” for payments for paid media, “gift_purchase” for gifts sent by the bot,
   * “premium_purchase” for Telegram Premium subscriptions gifted by the bot, “business_account_transfer” for
   * direct transfers from managed business accounts */
  String transaction_type;
  /* Information about the user */
  box<User> user;
  /* Optional. Information about the affiliate that received a commission via this transaction. Can be
   * available only for “invoice_payment” and “paid_media_payment” transactions. */
  box<AffiliateInfo> affiliate;
  /* Optional. Bot-specified invoice payload. Can be available only for “invoice_payment” transactions. */
  optional<String> invoice_payload;
  /* Optional. The duration of the paid subscription. Can be available only for “invoice_payment”
   * transactions. */
  optional<Integer> subscription_period;
  /* Optional. Information about the paid media bought by the user; for “paid_media_payment” transactions only
   */
  optional<arrayof<PaidMedia>> paid_media;
  /* Optional. Bot-specified paid media payload. Can be available only for “paid_media_payment” transactions.
   */
  optional<String> paid_media_payload;
  /* Optional. The gift sent to the user by the bot; for “gift_purchase” transactions only */
  box<Gift> gift;
  /* Optional. Number of months the gifted Telegram Premium subscription will be active for; for
   * “premium_purchase” transactions only */
  optional<Integer> premium_subscription_duration;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("transaction_type", true).case_("user", true).or_default(false);
  }
};

}  // namespace tgbm::api
