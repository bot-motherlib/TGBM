#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*This object describes the state of a revenue withdrawal operation. Currently, it can be one of*/
struct RevenueWithdrawalStateSucceeded {
  /* Date the withdrawal was completed in Unix time */
  Integer date;
  /* An HTTPS URL that can be used to see transaction details */
  String url;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("date", true).case_("url", true).or_default(false);
  }
};

}  // namespace tgbm::api
