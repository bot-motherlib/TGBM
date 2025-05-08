#pragma once

#include <tgbm/api/common.hpp>

namespace tgbm::api {

struct convert_gift_to_stars_request {
  /* Unique identifier of the business connection */
  String business_connection_id;
  /* Unique identifier of the regular gift that should be converted to Telegram Stars */
  String owned_gift_id;

  using return_type = bool;
  static constexpr file_info_e file_info = file_info_e::no;
  static constexpr std::string_view api_method_name = "convertGiftToStars";
  static constexpr http_method_e http_method = http_method_e::POST;

  void fill_nonfile_args(auto& body) const {
    body.arg("business_connection_id", business_connection_id);
    body.arg("owned_gift_id", owned_gift_id);
  }
};

}  // namespace tgbm::api
