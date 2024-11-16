#pragma once

#include <tgbm/api/common.hpp>

namespace tgbm::api {

struct set_my_description_request {
  /* New bot description; 0-512 characters. Pass an empty string to remove the dedicated description for the
   * given language. */
  optional<String> description;
  /* A two-letter ISO 639-1 language code. If empty, the description will be applied to all users for whose
   * language there is no dedicated description. */
  optional<String> language_code;

  using return_type = bool;
  static constexpr file_info_e file_info = file_info_e::no;
  static constexpr std::string_view api_method_name = "setMyDescription";
  static constexpr http_method_e http_method = http_method_e::POST;

  void fill_nonfile_args(auto& body) const {
    if (description)
      body.arg("description", *description);
    if (language_code)
      body.arg("language_code", *language_code);
  }
};

}  // namespace tgbm::api
