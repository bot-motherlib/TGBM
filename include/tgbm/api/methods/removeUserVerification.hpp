#pragma once

#include <tgbm/api/common.hpp>

namespace tgbm::api {

struct remove_user_verification_request {
  /* Unique identifier of the target user */
  Integer user_id;

  using return_type = bool;
  static constexpr file_info_e file_info = file_info_e::no;
  static constexpr std::string_view api_method_name = "removeUserVerification";
  static constexpr http_method_e http_method = http_method_e::POST;

  void fill_nonfile_args(auto& body) const {
    body.arg("user_id", user_id);
  }
};

}  // namespace tgbm::api
