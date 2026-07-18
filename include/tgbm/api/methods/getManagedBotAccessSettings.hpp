#pragma once

#include <tgbm/api/common.hpp>
#include <tgbm/api/types/BotAccessSettings.hpp>

namespace tgbm::api {

struct get_managed_bot_access_settings_request {
  /* User identifier of the managed bot whose access settings will be returned */
  Integer user_id;

  using return_type = BotAccessSettings;
  static constexpr file_info_e file_info = file_info_e::no;
  static constexpr std::string_view api_method_name = "getManagedBotAccessSettings";
  static constexpr http_method_e http_method = http_method_e::POST;

  void fill_nonfile_args(auto& body) const {
    body.arg("user_id", user_id);
  }
};

}  // namespace tgbm::api
