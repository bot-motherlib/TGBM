#pragma once

#include <tgbm/api/common.hpp>
#include <tgbm/api/types/GameHighScore.hpp>

namespace tgbm::api {

struct get_game_high_scores_request {
  /* Target user id */
  Integer user_id;
  /* Required if inline_message_id is not specified. Unique identifier for the target chat */
  optional<Integer> chat_id;
  /* Required if inline_message_id is not specified. Identifier of the sent message */
  optional<Integer> message_id;
  /* Required if chat_id and message_id are not specified. Identifier of the inline message */
  optional<String> inline_message_id;

  using return_type = arrayof<GameHighScore>;
  static constexpr file_info_e file_info = file_info_e::no;
  static constexpr std::string_view api_method_name = "getGameHighScores";
  static constexpr http_method_e http_method = http_method_e::POST;

  void fill_nonfile_args(auto& body) const {
    body.arg("user_id", user_id);
    if (chat_id)
      body.arg("chat_id", *chat_id);
    if (message_id)
      body.arg("message_id", *message_id);
    if (inline_message_id)
      body.arg("inline_message_id", *inline_message_id);
  }
};

}  // namespace tgbm::api
