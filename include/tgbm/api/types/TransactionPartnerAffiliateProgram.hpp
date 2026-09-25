#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*Describes the affiliate program that issued the affiliate commission received via this transaction.*/
struct TransactionPartnerAffiliateProgram {
  /* The number of Telegram Stars received by the bot for each 1000 Telegram Stars received by the affiliate
   * program sponsor from referred users */
  Integer commission_per_mille;
  /* Optional. Information about the bot that sponsored the affiliate program */
  box<User> sponsor_user;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("commission_per_mille", true).or_default(false);
  }
};

}  // namespace tgbm::api
