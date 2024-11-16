#pragma once

#include <tgbm/api/common.hpp>
#include <tgbm/api/types/Sticker.hpp>

namespace tgbm::api {

struct get_forum_topic_icon_stickers_request {
  using return_type = arrayof<Sticker>;
  static constexpr file_info_e file_info = file_info_e::no;
  static constexpr std::string_view api_method_name = "getForumTopicIconStickers";
  static constexpr http_method_e http_method = http_method_e::GET;
};

}  // namespace tgbm::api
