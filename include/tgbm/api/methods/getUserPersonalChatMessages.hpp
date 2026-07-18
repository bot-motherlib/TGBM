#pragma once

#include <tgbm/api/common.hpp>
#include <tgbm/api/types/Message.hpp>

namespace tgbm::api {

struct get_user_personal_chat_messages_request {
  /* Unique identifier for the target user */
  Integer user_id;
  /* The maximum number of messages to return; 1-20 */
  Integer limit;

  using return_type = arrayof<Message>;
  static constexpr file_info_e file_info = file_info_e::no;
  static constexpr std::string_view api_method_name = "getUserPersonalChatMessages";
  static constexpr http_method_e http_method = http_method_e::POST;

  void fill_nonfile_args(auto& body) const {
    body.arg("user_id", user_id);
    body.arg("limit", limit);
  }
};

}  // namespace tgbm::api
