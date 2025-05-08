#pragma once

#include <tgbm/api/common.hpp>

namespace tgbm::api {

struct transfer_business_account_stars_request {
  /* Unique identifier of the business connection */
  String business_connection_id;
  /* Number of Telegram Stars to transfer; 1-10000 */
  Integer star_count;

  using return_type = bool;
  static constexpr file_info_e file_info = file_info_e::no;
  static constexpr std::string_view api_method_name = "transferBusinessAccountStars";
  static constexpr http_method_e http_method = http_method_e::POST;

  void fill_nonfile_args(auto& body) const {
    body.arg("business_connection_id", business_connection_id);
    body.arg("star_count", star_count);
  }
};

}  // namespace tgbm::api
