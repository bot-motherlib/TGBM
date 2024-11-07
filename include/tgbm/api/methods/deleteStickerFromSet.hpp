#pragma once

#include "tgbm/api/common.hpp"

namespace tgbm::api {

struct delete_sticker_from_set_request {
  /* File identifier of the sticker */
  String sticker;

  using return_type = bool;
  static constexpr file_info_e file_info = file_info_e::no;
  static constexpr std::string_view api_method_name = "deleteStickerFromSet";
  static constexpr http_method_e http_method = http_method_e::POST;

  void fill_nonfile_args(auto& body) const {
    body.arg("sticker", sticker);
  }
};

}  // namespace tgbm::api
