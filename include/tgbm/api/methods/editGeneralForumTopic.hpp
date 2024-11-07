#pragma once

#include "tgbm/api/common.hpp"

namespace tgbm::api {

struct edit_general_forum_topic_request {
  /* Unique identifier for the target chat or username of the target supergroup (in the format
   * @supergroupusername) */
  int_or_str chat_id;
  /* New topic name, 1-128 characters */
  String name;

  using return_type = bool;
  static constexpr file_info_e file_info = file_info_e::no;
  static constexpr std::string_view api_method_name = "editGeneralForumTopic";
  static constexpr http_method_e http_method = http_method_e::POST;

  void fill_nonfile_args(auto& body) const {
    body.arg("chat_id", chat_id);
    body.arg("name", name);
  }
};

}  // namespace tgbm::api
