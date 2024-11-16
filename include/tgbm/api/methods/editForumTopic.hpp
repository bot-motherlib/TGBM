#pragma once

#include <tgbm/api/common.hpp>

namespace tgbm::api {

struct edit_forum_topic_request {
  /* Unique identifier for the target chat or username of the target supergroup (in the format
   * @supergroupusername) */
  int_or_str chat_id;
  /* Unique identifier for the target message thread of the forum topic */
  Integer message_thread_id;
  /* New topic name, 0-128 characters. If not specified or empty, the current name of the topic will be kept
   */
  optional<String> name;
  /* New unique identifier of the custom emoji shown as the topic icon. Use getForumTopicIconStickers to get
   * all allowed custom emoji identifiers. Pass an empty string to remove the icon. If not specified, the
   * current icon will be kept */
  optional<String> icon_custom_emoji_id;

  using return_type = bool;
  static constexpr file_info_e file_info = file_info_e::no;
  static constexpr std::string_view api_method_name = "editForumTopic";
  static constexpr http_method_e http_method = http_method_e::POST;

  void fill_nonfile_args(auto& body) const {
    body.arg("chat_id", chat_id);
    body.arg("message_thread_id", message_thread_id);
    if (name)
      body.arg("name", *name);
    if (icon_custom_emoji_id)
      body.arg("icon_custom_emoji_id", *icon_custom_emoji_id);
  }
};

}  // namespace tgbm::api
