#pragma once

#include <tgbm/api/common.hpp>

namespace tgbm::api {

struct remove_business_account_profile_photo_request {
  /* Unique identifier of the business connection */
  String business_connection_id;
  /* Pass True to remove the public photo, which is visible even if the main photo is hidden by the business
   * account's privacy settings. After the main photo is removed, the previous profile photo (if present)
   * becomes the main photo. */
  optional<bool> is_public;

  using return_type = bool;
  static constexpr file_info_e file_info = file_info_e::no;
  static constexpr std::string_view api_method_name = "removeBusinessAccountProfilePhoto";
  static constexpr http_method_e http_method = http_method_e::POST;

  void fill_nonfile_args(auto& body) const {
    body.arg("business_connection_id", business_connection_id);
    if (is_public)
      body.arg("is_public", *is_public);
  }
};

}  // namespace tgbm::api
