#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*Describes a transaction with payment for paid broadcasting.*/
struct TransactionPartnerTelegramApi {
  /* The number of successful requests that exceeded regular limits and were therefore billed */
  Integer request_count;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("request_count", true).or_default(false);
  }
};

}  // namespace tgbm::api
