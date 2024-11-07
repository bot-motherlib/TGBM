#pragma once

#include "tgbm/api/common.hpp"

namespace tgbm::api {

struct set_chat_sticker_set_request {
  /* Unique identifier for the target chat or username of the target supergroup (in the format
   * @supergroupusername) */
  int_or_str chat_id;
  /* Name of the sticker set to be set as the group sticker set */
  String sticker_set_name;

  using return_type = bool;
  static constexpr file_info_e file_info = file_info_e::no;
  static constexpr std::string_view api_method_name = "setChatStickerSet";
  static constexpr http_method_e http_method = http_method_e::POST;

  void fill_nonfile_args(auto& body) const {
    body.arg("chat_id", chat_id);
    body.arg("sticker_set_name", sticker_set_name);
  }
};

}  // namespace tgbm::api
