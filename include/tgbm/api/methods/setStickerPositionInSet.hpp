#pragma once

#include <tgbm/api/common.hpp>

namespace tgbm::api {

struct set_sticker_position_in_set_request {
  /* File identifier of the sticker */
  String sticker;
  /* New sticker position in the set, zero-based */
  Integer position;

  using return_type = bool;
  static constexpr file_info_e file_info = file_info_e::no;
  static constexpr std::string_view api_method_name = "setStickerPositionInSet";
  static constexpr http_method_e http_method = http_method_e::POST;

  void fill_nonfile_args(auto& body) const {
    body.arg("sticker", sticker);
    body.arg("position", position);
  }
};

}  // namespace tgbm::api
