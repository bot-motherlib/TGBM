#pragma once

#include "tgbm/api/common.hpp"
#include "tgbm/api/types/BotDescription.hpp"

namespace tgbm::api {

struct get_my_description_request {
  /* A two-letter ISO 639-1 language code or an empty string */
  optional<String> language_code;

  using return_type = BotDescription;
  static constexpr file_info_e file_info = file_info_e::no;
  static constexpr std::string_view api_method_name = "getMyDescription";
  static constexpr http_method_e http_method = http_method_e::POST;

  void fill_nonfile_args(auto& body) const {
    if (language_code)
      body.arg("language_code", *language_code);
  }
};

}  // namespace tgbm::api
