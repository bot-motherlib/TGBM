#pragma once

#include "tgbm/api/common.hpp"

namespace tgbm::api {

struct set_my_short_description_request {
  /* New short description for the bot; 0-120 characters. Pass an empty string to remove the dedicated short
   * description for the given language. */
  optional<String> short_description;
  /* A two-letter ISO 639-1 language code. If empty, the short description will be applied to all users for
   * whose language there is no dedicated short description. */
  optional<String> language_code;

  using return_type = bool;
  static constexpr file_info_e file_info = file_info_e::no;
  static constexpr std::string_view api_method_name = "setMyShortDescription";
  static constexpr http_method_e http_method = http_method_e::POST;

  void fill_nonfile_args(auto& body) const {
    if (short_description)
      body.arg("short_description", *short_description);
    if (language_code)
      body.arg("language_code", *language_code);
  }
};

}  // namespace tgbm::api
