#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*This object describes the source of a transaction, or its recipient for outgoing transactions. Currently, it
 * can be one of*/
struct TransactionPartnerOther {
  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).or_default(false);
  }

  bool operator==(const TransactionPartnerOther&) const;
  std::strong_ordering operator<=>(const TransactionPartnerOther&) const;
};

}  // namespace tgbm::api
