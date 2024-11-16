#pragma once

#include <tgbm/api/common.hpp>

namespace tgbm::api {

struct delete_forum_topic_request {
  /* Unique identifier for the target chat or username of the target supergroup (in the format
   * @supergroupusername) */
  int_or_str chat_id;
  /* Unique identifier for the target message thread of the forum topic */
  Integer message_thread_id;

  using return_type = bool;
  static constexpr file_info_e file_info = file_info_e::no;
  static constexpr std::string_view api_method_name = "deleteForumTopic";
  static constexpr http_method_e http_method = http_method_e::POST;

  void fill_nonfile_args(auto& body) const {
    body.arg("chat_id", chat_id);
    body.arg("message_thread_id", message_thread_id);
  }
};

}  // namespace tgbm::api
