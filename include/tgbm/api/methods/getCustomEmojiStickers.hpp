#pragma once

#include <tgbm/api/common.hpp>
#include <tgbm/api/types/Sticker.hpp>

namespace tgbm::api {

struct get_custom_emoji_stickers_request {
  /* A JSON-serialized list of custom emoji identifiers. At most 200 custom emoji identifiers can be
   * specified. */
  arrayof<String> custom_emoji_ids;

  using return_type = arrayof<Sticker>;
  static constexpr file_info_e file_info = file_info_e::no;
  static constexpr std::string_view api_method_name = "getCustomEmojiStickers";
  static constexpr http_method_e http_method = http_method_e::POST;

  void fill_nonfile_args(auto& body) const {
    body.arg("custom_emoji_ids", custom_emoji_ids);
  }
};

}  // namespace tgbm::api
