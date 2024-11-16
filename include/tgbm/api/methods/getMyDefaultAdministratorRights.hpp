#pragma once

#include <tgbm/api/common.hpp>
#include <tgbm/api/types/ChatAdministratorRights.hpp>

namespace tgbm::api {

struct get_my_default_administrator_rights_request {
  /* Pass True to get default administrator rights of the bot in channels. Otherwise, default administrator
   * rights of the bot for groups and supergroups will be returned. */
  optional<bool> for_channels;

  using return_type = ChatAdministratorRights;
  static constexpr file_info_e file_info = file_info_e::no;
  static constexpr std::string_view api_method_name = "getMyDefaultAdministratorRights";
  static constexpr http_method_e http_method = http_method_e::POST;

  void fill_nonfile_args(auto& body) const {
    if (for_channels)
      body.arg("for_channels", *for_channels);
  }
};

}  // namespace tgbm::api
