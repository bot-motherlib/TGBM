#pragma once

#include <tgbm/api/common.hpp>
#include <tgbm/api/types/StarAmount.hpp>

namespace tgbm::api {

struct get_my_star_balance_request {
  using return_type = StarAmount;
  static constexpr file_info_e file_info = file_info_e::no;
  static constexpr std::string_view api_method_name = "getMyStarBalance";
  static constexpr http_method_e http_method = http_method_e::GET;
};

}  // namespace tgbm::api
