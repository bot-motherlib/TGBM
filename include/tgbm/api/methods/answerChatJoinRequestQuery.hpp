#pragma once

#include <tgbm/api/common.hpp>

namespace tgbm::api {

struct answer_chat_join_request_query_request {
  /* Unique identifier of the join request query */
  String chat_join_request_query_id;
  /* Result of the query. Must be either “approve” to allow the user to join the chat, “decline” to disallow
   * the user to join the chat, or “queue” to leave the decision to other administrators. */
  String result;

  using return_type = bool;
  static constexpr file_info_e file_info = file_info_e::no;
  static constexpr std::string_view api_method_name = "answerChatJoinRequestQuery";
  static constexpr http_method_e http_method = http_method_e::POST;

  void fill_nonfile_args(auto& body) const {
    body.arg("chat_join_request_query_id", chat_join_request_query_id);
    body.arg("result", result);
  }
};

}  // namespace tgbm::api
