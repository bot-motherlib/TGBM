#pragma once

#include <tgbm/api/common.hpp>

namespace tgbm::api {

struct set_business_account_name_request {
  /* Unique identifier of the business connection */
  String business_connection_id;
  /* The new value of the first name for the business account; 1-64 characters */
  String first_name;
  /* The new value of the last name for the business account; 0-64 characters */
  optional<String> last_name;

  using return_type = bool;
  static constexpr file_info_e file_info = file_info_e::no;
  static constexpr std::string_view api_method_name = "setBusinessAccountName";
  static constexpr http_method_e http_method = http_method_e::POST;

  void fill_nonfile_args(auto& body) const {
    body.arg("business_connection_id", business_connection_id);
    body.arg("first_name", first_name);
    if (last_name)
      body.arg("last_name", *last_name);
  }
};

}  // namespace tgbm::api
