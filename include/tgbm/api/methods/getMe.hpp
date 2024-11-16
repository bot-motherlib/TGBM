#pragma once

#include <tgbm/api/common.hpp>
#include <tgbm/api/types/User.hpp>

namespace tgbm::api {

struct get_me_request {
  using return_type = User;
  static constexpr file_info_e file_info = file_info_e::no;
  static constexpr std::string_view api_method_name = "getMe";
  static constexpr http_method_e http_method = http_method_e::GET;
};

}  // namespace tgbm::api
