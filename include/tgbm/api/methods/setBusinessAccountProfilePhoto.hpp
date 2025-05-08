#pragma once

#include <tgbm/api/common.hpp>
#include <tgbm/api/types/InputProfilePhoto.hpp>

namespace tgbm::api {

struct set_business_account_profile_photo_request {
  /* Unique identifier of the business connection */
  String business_connection_id;
  /* The new profile photo to set */
  box<InputProfilePhoto> photo;
  /* Pass True to set the public photo, which will be visible even if the main photo is hidden by the business
   * account's privacy settings. An account can have only one public photo. */
  optional<bool> is_public;

  using return_type = bool;
  static constexpr file_info_e file_info = file_info_e::no;
  static constexpr std::string_view api_method_name = "setBusinessAccountProfilePhoto";
  static constexpr http_method_e http_method = http_method_e::POST;

  void fill_nonfile_args(auto& body) const {
    body.arg("business_connection_id", business_connection_id);
    body.arg("photo", photo);
    if (is_public)
      body.arg("is_public", *is_public);
  }
};

}  // namespace tgbm::api
