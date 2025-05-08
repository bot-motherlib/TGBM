#pragma once

#include <tgbm/api/common.hpp>
#include <tgbm/api/types/StarAmount.hpp>

namespace tgbm::api {

struct get_business_account_star_balance_request {
  /* Unique identifier of the business connection */
  String business_connection_id;

  using return_type = StarAmount;
  static constexpr file_info_e file_info = file_info_e::no;
  static constexpr std::string_view api_method_name = "getBusinessAccountStarBalance";
  static constexpr http_method_e http_method = http_method_e::POST;

  void fill_nonfile_args(auto& body) const {
    body.arg("business_connection_id", business_connection_id);
  }
};

}  // namespace tgbm::api
