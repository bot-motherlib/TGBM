#pragma once

#include "tgbm/api/common.hpp"
#include "tgbm/api/types/MenuButton.hpp"

namespace tgbm::api {

struct get_chat_menu_button_request {
  /* Unique identifier for the target private chat. If not specified, default bot's menu button will be
   * returned */
  optional<Integer> chat_id;

  using return_type = MenuButton;
  static constexpr file_info_e file_info = file_info_e::no;
  static constexpr std::string_view api_method_name = "getChatMenuButton";
  static constexpr http_method_e http_method = http_method_e::POST;

  void fill_nonfile_args(auto& body) const {
    if (chat_id)
      body.arg("chat_id", *chat_id);
  }
};

}  // namespace tgbm::api
