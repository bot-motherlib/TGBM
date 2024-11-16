#pragma once

#include <tgbm/api/common.hpp>
#include <tgbm/api/types/BusinessConnection.hpp>

namespace tgbm::api {

struct get_business_connection_request {
  /* Unique identifier of the business connection */
  String business_connection_id;

  using return_type = BusinessConnection;
  static constexpr file_info_e file_info = file_info_e::no;
  static constexpr std::string_view api_method_name = "getBusinessConnection";
  static constexpr http_method_e http_method = http_method_e::POST;

  void fill_nonfile_args(auto& body) const {
    body.arg("business_connection_id", business_connection_id);
  }
};

}  // namespace tgbm::api
