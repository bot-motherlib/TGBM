#pragma once

#include <tgbm/api/common.hpp>
#include <tgbm/api/types/InputRichMessage.hpp>

namespace tgbm::api {

struct send_rich_message_draft_request {
  /* Unique identifier for the target private chat */
  Integer chat_id;
  /* Unique identifier of the message draft; must be non-zero. Changes to drafts with the same identifier are
   * animated. */
  Integer draft_id;
  /* The partial message to be streamed. Direct upload of new files isn't supported. */
  box<InputRichMessage> rich_message;
  /* Unique identifier for the target message thread */
  optional<Integer> message_thread_id;

  using return_type = bool;
  static constexpr file_info_e file_info = file_info_e::no;
  static constexpr std::string_view api_method_name = "sendRichMessageDraft";
  static constexpr http_method_e http_method = http_method_e::POST;

  void fill_nonfile_args(auto& body) const {
    body.arg("chat_id", chat_id);
    if (message_thread_id)
      body.arg("message_thread_id", *message_thread_id);
    body.arg("draft_id", draft_id);
    body.arg("rich_message", rich_message);
  }
};

}  // namespace tgbm::api
