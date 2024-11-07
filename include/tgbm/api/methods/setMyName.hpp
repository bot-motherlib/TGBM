#pragma once

#include "tgbm/api/common.hpp"

namespace tgbm::api {

struct set_my_name_request {
  /* New bot name; 0-64 characters. Pass an empty string to remove the dedicated name for the given language.
   */
  optional<String> name;
  /* A two-letter ISO 639-1 language code. If empty, the name will be shown to all users for whose language
   * there is no dedicated name. */
  optional<String> language_code;

  using return_type = bool;
  static constexpr file_info_e file_info = file_info_e::no;
  static constexpr std::string_view api_method_name = "setMyName";
  static constexpr http_method_e http_method = http_method_e::POST;

  void fill_nonfile_args(auto& body) const {
    if (name)
      body.arg("name", *name);
    if (language_code)
      body.arg("language_code", *language_code);
  }
};

}  // namespace tgbm::api
