#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*Contains information about the affiliate that received a commission via this transaction.*/
struct AffiliateInfo {
  /* The number of Telegram Stars received by the affiliate for each 1000 Telegram Stars received by the bot
   * from referred users */
  Integer commission_per_mille;
  /* Integer amount of Telegram Stars received by the affiliate from the transaction, rounded to 0; can be
   * negative for refunds */
  Integer amount;
  /* Optional. The bot or the user that received an affiliate commission if it was received by a bot or a user
   */
  box<User> affiliate_user;
  /* Optional. The chat that received an affiliate commission if it was received by a chat */
  box<Chat> affiliate_chat;
  /* Optional. The number of 1/1000000000 shares of Telegram Stars received by the affiliate; from -999999999
   * to 999999999; can be negative for refunds */
  optional<Integer> nanostar_amount;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name)
        .case_("commission_per_mille", true)
        .case_("amount", true)
        .or_default(false);
  }
};

}  // namespace tgbm::api
