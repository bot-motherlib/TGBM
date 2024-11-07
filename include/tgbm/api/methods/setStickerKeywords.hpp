#pragma once

#include "tgbm/api/common.hpp"

namespace tgbm::api {

struct set_sticker_keywords_request {
  /* File identifier of the sticker */
  String sticker;
  /* A JSON-serialized list of 0-20 search keywords for the sticker with total length of up to 64 characters
   */
  optional<arrayof<String>> keywords;

  using return_type = bool;
  static constexpr file_info_e file_info = file_info_e::no;
  static constexpr std::string_view api_method_name = "setStickerKeywords";
  static constexpr http_method_e http_method = http_method_e::POST;

  void fill_nonfile_args(auto& body) const {
    body.arg("sticker", sticker);
    if (keywords)
      body.arg("keywords", *keywords);
  }
};

}  // namespace tgbm::api
