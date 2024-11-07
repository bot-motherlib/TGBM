#pragma once

#include "tgbm/api/common.hpp"
#include "tgbm/api/types/MaskPosition.hpp"

namespace tgbm::api {

struct set_sticker_mask_position_request {
  /* File identifier of the sticker */
  String sticker;
  /* A JSON-serialized object with the position where the mask should be placed on faces. Omit the parameter
   * to remove the mask position. */
  box<MaskPosition> mask_position;

  using return_type = bool;
  static constexpr file_info_e file_info = file_info_e::no;
  static constexpr std::string_view api_method_name = "setStickerMaskPosition";
  static constexpr http_method_e http_method = http_method_e::POST;

  void fill_nonfile_args(auto& body) const {
    body.arg("sticker", sticker);
    if (mask_position)
      body.arg("mask_position", *mask_position);
  }
};

}  // namespace tgbm::api
