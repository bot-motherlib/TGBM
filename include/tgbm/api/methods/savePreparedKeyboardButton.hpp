#pragma once

#include <tgbm/api/common.hpp>
#include <tgbm/api/types/KeyboardButton.hpp>
#include <tgbm/api/types/PreparedKeyboardButton.hpp>

namespace tgbm::api {

struct save_prepared_keyboard_button_request {
  /* Unique identifier of the target user that can use the button */
  Integer user_id;
  /* A JSON-serialized object describing the button to be saved. The button must be of the type request_users,
   * request_chat, or request_managed_bot */
  box<KeyboardButton> button;

  using return_type = PreparedKeyboardButton;
  static constexpr file_info_e file_info = file_info_e::no;
  static constexpr std::string_view api_method_name = "savePreparedKeyboardButton";
  static constexpr http_method_e http_method = http_method_e::POST;

  void fill_nonfile_args(auto& body) const {
    body.arg("user_id", user_id);
    body.arg("button", button);
  }
};

}  // namespace tgbm::api
