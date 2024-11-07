#pragma once

#include "tgbm/api/common.hpp"

namespace tgbm::api {

struct delete_message_request {
  /* Unique identifier for the target chat or username of the target channel (in the format @channelusername)
   */
  int_or_str chat_id;
  /* Identifier of the message to delete */
  Integer message_id;

  using return_type = bool;
  static constexpr file_info_e file_info = file_info_e::no;
  static constexpr std::string_view api_method_name = "deleteMessage";
  static constexpr http_method_e http_method = http_method_e::POST;

  void fill_nonfile_args(auto& body) const {
    body.arg("chat_id", chat_id);
    body.arg("message_id", message_id);
  }
};

}  // namespace tgbm::api
