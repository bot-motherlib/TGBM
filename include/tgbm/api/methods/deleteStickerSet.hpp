#pragma once

#include "tgbm/api/common.hpp"

namespace tgbm::api {

struct delete_sticker_set_request {
  /* Sticker set name */
  String name;

  using return_type = bool;
  static constexpr file_info_e file_info = file_info_e::no;
  static constexpr std::string_view api_method_name = "deleteStickerSet";
  static constexpr http_method_e http_method = http_method_e::POST;

  void fill_nonfile_args(auto& body) const {
    body.arg("name", name);
  }
};

}  // namespace tgbm::api
