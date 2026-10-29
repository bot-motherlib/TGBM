#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*Describes a withdrawal transaction to the Telegram Ads platform.*/
struct TransactionPartnerTelegramAds {
  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).or_default(false);
  }

  bool operator==(const TransactionPartnerTelegramAds&) const;
  std::strong_ordering operator<=>(const TransactionPartnerTelegramAds&) const;
};

}  // namespace tgbm::api
