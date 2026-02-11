#pragma once

#include <tgbm/api/common.hpp>

namespace tgbm::api {

struct remove_my_profile_photo_request {
  using return_type = bool;
  static constexpr file_info_e file_info = file_info_e::no;
  static constexpr std::string_view api_method_name = "removeMyProfilePhoto";
  static constexpr http_method_e http_method = http_method_e::GET;
};

}  // namespace tgbm::api
