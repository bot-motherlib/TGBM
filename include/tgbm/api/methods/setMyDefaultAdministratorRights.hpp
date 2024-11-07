#pragma once

#include "tgbm/api/common.hpp"
#include "tgbm/api/types/ChatAdministratorRights.hpp"

namespace tgbm::api {

struct set_my_default_administrator_rights_request {
  /* A JSON-serialized object describing new default administrator rights. If not specified, the default
   * administrator rights will be cleared. */
  box<ChatAdministratorRights> rights;
  /* Pass True to change the default administrator rights of the bot in channels. Otherwise, the default
   * administrator rights of the bot for groups and supergroups will be changed. */
  optional<bool> for_channels;

  using return_type = bool;
  static constexpr file_info_e file_info = file_info_e::no;
  static constexpr std::string_view api_method_name = "setMyDefaultAdministratorRights";
  static constexpr http_method_e http_method = http_method_e::POST;

  void fill_nonfile_args(auto& body) const {
    if (rights)
      body.arg("rights", *rights);
    if (for_channels)
      body.arg("for_channels", *for_channels);
  }
};

}  // namespace tgbm::api
