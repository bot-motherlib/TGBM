#pragma once

#include <tgbm/api/common.hpp>
#include <tgbm/api/types/InputProfilePhoto.hpp>

namespace tgbm::api {

struct set_my_profile_photo_request {
  /* The new profile photo to set */
  box<InputProfilePhoto> photo;

  using return_type = bool;
  static constexpr file_info_e file_info = file_info_e::no;
  static constexpr std::string_view api_method_name = "setMyProfilePhoto";
  static constexpr http_method_e http_method = http_method_e::POST;

  void fill_nonfile_args(auto& body) const {
    body.arg("photo", photo);
  }
};

}  // namespace tgbm::api
