#pragma once

#include <tgbm/api/common.hpp>

namespace tgbm::api {

struct set_business_account_username_request {
  /* Unique identifier of the business connection */
  String business_connection_id;
  /* The new value of the username for the business account; 0-32 characters */
  optional<String> username;

  using return_type = bool;
  static constexpr file_info_e file_info = file_info_e::no;
  static constexpr std::string_view api_method_name = "setBusinessAccountUsername";
  static constexpr http_method_e http_method = http_method_e::POST;

  void fill_nonfile_args(auto& body) const {
    body.arg("business_connection_id", business_connection_id);
    if (username)
      body.arg("username", *username);
  }
};

}  // namespace tgbm::api
