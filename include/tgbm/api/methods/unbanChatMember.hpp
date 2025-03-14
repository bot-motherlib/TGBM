#pragma once

#include <tgbm/api/common.hpp>

namespace tgbm::api {

struct unban_chat_member_request {
  /* Unique identifier for the target group or username of the target supergroup or channel (in the format
   * @channelusername) */
  int_or_str chat_id;
  /* Unique identifier of the target user */
  Integer user_id;
  /* Do nothing if the user is not banned */
  optional<bool> only_if_banned;

  using return_type = bool;
  static constexpr file_info_e file_info = file_info_e::no;
  static constexpr std::string_view api_method_name = "unbanChatMember";
  static constexpr http_method_e http_method = http_method_e::POST;

  void fill_nonfile_args(auto& body) const {
    body.arg("chat_id", chat_id);
    body.arg("user_id", user_id);
    if (only_if_banned)
      body.arg("only_if_banned", *only_if_banned);
  }
};

}  // namespace tgbm::api
