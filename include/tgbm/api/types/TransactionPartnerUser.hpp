#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*This object describes the source of a transaction, or its recipient for outgoing transactions. Currently, it
 * can be one of*/
struct TransactionPartnerUser {
  /* Information about the user */
  box<User> user;
  /* Optional. Information about the affiliate that received a commission via this transaction */
  box<AffiliateInfo> affiliate;
  /* Optional. Bot-specified invoice payload */
  optional<String> invoice_payload;
  /* Optional. The duration of the paid subscription */
  optional<Integer> subscription_period;
  /* Optional. Information about the paid media bought by the user */
  optional<arrayof<PaidMedia>> paid_media;
  /* Optional. Bot-specified paid media payload */
  optional<String> paid_media_payload;
  /* Optional. The gift sent to the user by the bot */
  box<Gift> gift;

  constexpr static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("user", true).or_default(false);
  }
};

}  // namespace tgbm::api
