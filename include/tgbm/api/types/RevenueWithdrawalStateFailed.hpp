#pragma once

#include "all_fwd.hpp"

namespace tgbm::api {

/*This object describes the state of a revenue withdrawal operation. Currently, it can be one of*/
struct RevenueWithdrawalStateFailed {
  consteval static bool is_mandatory_field(std::string_view name) {
    return utils::string_switch<bool>(name).or_default(false);
  }
};

}  // namespace tgbm::api