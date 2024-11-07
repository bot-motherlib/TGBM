#pragma once

#include "all_fwd.hpp"

namespace tgbm::api {

/*This object describes the source of a transaction, or its recipient for outgoing transactions. Currently, it
 * can be one of*/
struct TransactionPartnerFragment {
  /* Optional. State of the transaction if the transaction is outgoing */
  box<RevenueWithdrawalState> withdrawal_state;

  consteval static bool is_mandatory_field(std::string_view name) {
    return utils::string_switch<bool>(name).or_default(false);
  }
};

}  // namespace tgbm::api
