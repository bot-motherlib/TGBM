#pragma once

#include "tgbm/api/common.hpp"
#include "tgbm/api/types/Message.hpp"

namespace tgbm::api {

struct set_game_score_request {
  /* User identifier */
  Integer user_id;
  /* New score, must be non-negative */
  Integer score;
  /* Pass True if the high score is allowed to decrease. This can be useful when fixing mistakes or banning
   * cheaters */
  optional<bool> force;
  /* Pass True if the game message should not be automatically edited to include the current scoreboard */
  optional<bool> disable_edit_message;
  /* Required if inline_message_id is not specified. Unique identifier for the target chat */
  optional<Integer> chat_id;
  /* Required if inline_message_id is not specified. Identifier of the sent message */
  optional<Integer> message_id;
  /* Required if chat_id and message_id are not specified. Identifier of the inline message */
  optional<String> inline_message_id;

  using return_type = oneof<bool, Message>;
  static constexpr file_info_e file_info = file_info_e::no;
  static constexpr std::string_view api_method_name = "setGameScore";
  static constexpr http_method_e http_method = http_method_e::POST;

  void fill_nonfile_args(auto& body) const {
    body.arg("user_id", user_id);
    body.arg("score", score);
    if (force)
      body.arg("force", *force);
    if (disable_edit_message)
      body.arg("disable_edit_message", *disable_edit_message);
    if (chat_id)
      body.arg("chat_id", *chat_id);
    if (message_id)
      body.arg("message_id", *message_id);
    if (inline_message_id)
      body.arg("inline_message_id", *inline_message_id);
  }
};

}  // namespace tgbm::api
