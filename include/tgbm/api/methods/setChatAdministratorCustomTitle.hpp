#pragma once

#include <tgbm/api/common.hpp>

namespace tgbm::api {

struct set_chat_administrator_custom_title_request {
  /* Unique identifier for the target chat or username of the target supergroup (in the format
   * @supergroupusername) */
  int_or_str chat_id;
  /* Unique identifier of the target user */
  Integer user_id;
  /* New custom title for the administrator; 0-16 characters, emoji are not allowed */
  String custom_title;

  using return_type = bool;
  static constexpr file_info_e file_info = file_info_e::no;
  static constexpr std::string_view api_method_name = "setChatAdministratorCustomTitle";
  static constexpr http_method_e http_method = http_method_e::POST;

  void fill_nonfile_args(auto& body) const {
    body.arg("chat_id", chat_id);
    body.arg("user_id", user_id);
    body.arg("custom_title", custom_title);
  }
};

}  // namespace tgbm::api
