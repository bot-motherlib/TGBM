#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*The withdrawal is in progress.*/
struct RevenueWithdrawalStatePending {
  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).or_default(false);
  }
};

}  // namespace tgbm::api
