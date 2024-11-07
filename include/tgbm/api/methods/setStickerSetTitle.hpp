#pragma once

#include "tgbm/api/common.hpp"

namespace tgbm::api {

struct set_sticker_set_title_request {
  /* Sticker set name */
  String name;
  /* Sticker set title, 1-64 characters */
  String title;

  using return_type = bool;
  static constexpr file_info_e file_info = file_info_e::no;
  static constexpr std::string_view api_method_name = "setStickerSetTitle";
  static constexpr http_method_e http_method = http_method_e::POST;

  void fill_nonfile_args(auto& body) const {
    body.arg("name", name);
    body.arg("title", title);
  }
};

}  // namespace tgbm::api
