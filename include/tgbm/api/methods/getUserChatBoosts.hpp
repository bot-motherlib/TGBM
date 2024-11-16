#pragma once

#include <tgbm/api/common.hpp>
#include <tgbm/api/types/UserChatBoosts.hpp>

namespace tgbm::api {

struct get_user_chat_boosts_request {
  /* Unique identifier for the chat or username of the channel (in the format @channelusername) */
  int_or_str chat_id;
  /* Unique identifier of the target user */
  Integer user_id;

  using return_type = UserChatBoosts;
  static constexpr file_info_e file_info = file_info_e::no;
  static constexpr std::string_view api_method_name = "getUserChatBoosts";
  static constexpr http_method_e http_method = http_method_e::POST;

  void fill_nonfile_args(auto& body) const {
    body.arg("chat_id", chat_id);
    body.arg("user_id", user_id);
  }
};

}  // namespace tgbm::api
