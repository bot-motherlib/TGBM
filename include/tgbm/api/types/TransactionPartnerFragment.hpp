#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*Describes a withdrawal transaction with Fragment.*/
struct TransactionPartnerFragment {
  /* Optional. State of the transaction if the transaction is outgoing */
  box<RevenueWithdrawalState> withdrawal_state;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).or_default(false);
  }
};

}  // namespace tgbm::api
