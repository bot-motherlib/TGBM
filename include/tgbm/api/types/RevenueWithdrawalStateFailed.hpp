#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*The withdrawal failed and the transaction was refunded.*/
struct RevenueWithdrawalStateFailed {
  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).or_default(false);
  }

  bool operator==(const RevenueWithdrawalStateFailed&) const;
  std::strong_ordering operator<=>(const RevenueWithdrawalStateFailed&) const;
};

}  // namespace tgbm::api
