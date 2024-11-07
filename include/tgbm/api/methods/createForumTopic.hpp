#pragma once

#include "tgbm/api/common.hpp"
#include "tgbm/api/types/ForumTopic.hpp"

namespace tgbm::api {

struct create_forum_topic_request {
  /* Unique identifier for the target chat or username of the target supergroup (in the format
   * @supergroupusername) */
  int_or_str chat_id;
  /* Topic name, 1-128 characters */
  String name;
  /* Color of the topic icon in RGB format. Currently, must be one of 7322096 (0x6FB9F0), 16766590 (0xFFD67E),
   * 13338331 (0xCB86DB), 9367192 (0x8EEE98), 16749490 (0xFF93B2), or 16478047 (0xFB6F5F) */
  optional<Integer> icon_color;
  /* Unique identifier of the custom emoji shown as the topic icon. Use getForumTopicIconStickers to get all
   * allowed custom emoji identifiers. */
  optional<String> icon_custom_emoji_id;

  using return_type = ForumTopic;
  static constexpr file_info_e file_info = file_info_e::no;
  static constexpr std::string_view api_method_name = "createForumTopic";
  static constexpr http_method_e http_method = http_method_e::POST;

  void fill_nonfile_args(auto& body) const {
    body.arg("chat_id", chat_id);
    body.arg("name", name);
    if (icon_color)
      body.arg("icon_color", *icon_color);
    if (icon_custom_emoji_id)
      body.arg("icon_custom_emoji_id", *icon_custom_emoji_id);
  }
};

}  // namespace tgbm::api
