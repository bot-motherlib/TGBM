#pragma once

#include <tgbm/api/common.hpp>

namespace tgbm::api {

struct delete_ephemeral_message_request {
  /* Unique identifier for the target chat or username of the target supergroup in the format @username */
  int_or_str chat_id;
  /* Identifier of the user who received the message */
  Integer receiver_user_id;
  /* Identifier of the ephemeral message to delete */
  Integer ephemeral_message_id;

  using return_type = bool;
  static constexpr file_info_e file_info = file_info_e::no;
  static constexpr std::string_view api_method_name = "deleteEphemeralMessage";
  static constexpr http_method_e http_method = http_method_e::POST;

  void fill_nonfile_args(auto& body) const {
    body.arg("chat_id", chat_id);
    body.arg("receiver_user_id", receiver_user_id);
    body.arg("ephemeral_message_id", ephemeral_message_id);
  }
};

}  // namespace tgbm::api
