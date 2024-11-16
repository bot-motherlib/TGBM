#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*Contains a list of Telegram Star transactions.*/
struct StarTransactions {
  /* The list of transactions */
  arrayof<StarTransaction> transactions;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).case_("transactions", true).or_default(false);
  }
};

}  // namespace tgbm::api
