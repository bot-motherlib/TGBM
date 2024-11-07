#pragma once

#include "tgbm/api/common.hpp"
#include "tgbm/api/types/StickerSet.hpp"

namespace tgbm::api {

struct get_sticker_set_request {
  /* Name of the sticker set */
  String name;

  using return_type = StickerSet;
  static constexpr file_info_e file_info = file_info_e::no;
  static constexpr std::string_view api_method_name = "getStickerSet";
  static constexpr http_method_e http_method = http_method_e::POST;

  void fill_nonfile_args(auto& body) const {
    body.arg("name", name);
  }
};

}  // namespace tgbm::api
