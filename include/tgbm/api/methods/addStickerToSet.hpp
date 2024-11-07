#pragma once

#include "tgbm/api/common.hpp"
#include "tgbm/api/types/InputSticker.hpp"

namespace tgbm::api {

struct add_sticker_to_set_request {
  /* User identifier of sticker set owner */
  Integer user_id;
  /* Sticker set name */
  String name;
  /* A JSON-serialized object with information about the added sticker. If exactly the same sticker had
   * already been added to the set, then the set isn't changed. */
  box<InputSticker> sticker;

  using return_type = bool;
  static constexpr file_info_e file_info = file_info_e::no;
  static constexpr std::string_view api_method_name = "addStickerToSet";
  static constexpr http_method_e http_method = http_method_e::POST;

  void fill_nonfile_args(auto& body) const {
    body.arg("user_id", user_id);
    body.arg("name", name);
    body.arg("sticker", sticker);
  }
};

}  // namespace tgbm::api
