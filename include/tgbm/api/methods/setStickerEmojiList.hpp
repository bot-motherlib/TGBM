#pragma once

#include <tgbm/api/common.hpp>

namespace tgbm::api {

struct set_sticker_emoji_list_request {
  /* File identifier of the sticker */
  String sticker;
  /* A JSON-serialized list of 1-20 emoji associated with the sticker */
  arrayof<String> emoji_list;

  using return_type = bool;
  static constexpr file_info_e file_info = file_info_e::no;
  static constexpr std::string_view api_method_name = "setStickerEmojiList";
  static constexpr http_method_e http_method = http_method_e::POST;

  void fill_nonfile_args(auto& body) const {
    body.arg("sticker", sticker);
    body.arg("emoji_list", emoji_list);
  }
};

}  // namespace tgbm::api
