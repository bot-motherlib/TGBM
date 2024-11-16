#pragma once

#include <tgbm/api/common.hpp>

namespace tgbm::api {

struct ban_chat_sender_chat_request {
  /* Unique identifier for the target chat or username of the target channel (in the format @channelusername)
   */
  int_or_str chat_id;
  /* Unique identifier of the target sender chat */
  Integer sender_chat_id;

  using return_type = bool;
  static constexpr file_info_e file_info = file_info_e::no;
  static constexpr std::string_view api_method_name = "banChatSenderChat";
  static constexpr http_method_e http_method = http_method_e::POST;

  void fill_nonfile_args(auto& body) const {
    body.arg("chat_id", chat_id);
    body.arg("sender_chat_id", sender_chat_id);
  }
};

}  // namespace tgbm::api
