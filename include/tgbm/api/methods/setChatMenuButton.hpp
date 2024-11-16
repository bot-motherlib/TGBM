#pragma once

#include <tgbm/api/common.hpp>
#include <tgbm/api/types/MenuButton.hpp>

namespace tgbm::api {

struct set_chat_menu_button_request {
  /* Unique identifier for the target private chat. If not specified, default bot's menu button will be
   * changed */
  optional<Integer> chat_id;
  /* A JSON-serialized object for the bot's new menu button. Defaults to MenuButtonDefault */
  box<MenuButton> menu_button;

  using return_type = bool;
  static constexpr file_info_e file_info = file_info_e::no;
  static constexpr std::string_view api_method_name = "setChatMenuButton";
  static constexpr http_method_e http_method = http_method_e::POST;

  void fill_nonfile_args(auto& body) const {
    if (chat_id)
      body.arg("chat_id", *chat_id);
    if (menu_button)
      body.arg("menu_button", *menu_button);
  }
};

}  // namespace tgbm::api
