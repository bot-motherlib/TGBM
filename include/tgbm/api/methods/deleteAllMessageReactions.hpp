#pragma once

#include <tgbm/api/common.hpp>

namespace tgbm::api {

struct delete_all_message_reactions_request {
  /* Unique identifier for the target chat or username of the target supergroup in the format @username */
  int_or_str chat_id;
  /* Identifier of the user whose reactions will be removed, if the reactions were added by a user */
  optional<Integer> user_id;
  /* Identifier of the chat whose reactions will be removed, if the reactions were added by a chat */
  optional<Integer> actor_chat_id;

  using return_type = bool;
  static constexpr file_info_e file_info = file_info_e::no;
  static constexpr std::string_view api_method_name = "deleteAllMessageReactions";
  static constexpr http_method_e http_method = http_method_e::POST;

  void fill_nonfile_args(auto& body) const {
    body.arg("chat_id", chat_id);
    if (user_id)
      body.arg("user_id", *user_id);
    if (actor_chat_id)
      body.arg("actor_chat_id", *actor_chat_id);
  }
};

}  // namespace tgbm::api
