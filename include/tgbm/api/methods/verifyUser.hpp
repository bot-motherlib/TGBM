#pragma once

#include <tgbm/api/common.hpp>

namespace tgbm::api {

struct verify_user_request {
  /* Unique identifier of the target user */
  Integer user_id;
  /* Custom description for the verification; 0-70 characters. Must be empty if the organization isn't allowed
   * to provide a custom verification description. */
  optional<String> custom_description;

  using return_type = bool;
  static constexpr file_info_e file_info = file_info_e::no;
  static constexpr std::string_view api_method_name = "verifyUser";
  static constexpr http_method_e http_method = http_method_e::POST;

  void fill_nonfile_args(auto& body) const {
    body.arg("user_id", user_id);
    if (custom_description)
      body.arg("custom_description", *custom_description);
  }
};

}  // namespace tgbm::api
