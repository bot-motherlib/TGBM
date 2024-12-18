#pragma once

#include <tgbm/api/common.hpp>
#include <tgbm/api/types/InlineQueryResult.hpp>
#include <tgbm/api/types/PreparedInlineMessage.hpp>

namespace tgbm::api {

struct save_prepared_inline_message_request {
  /* Unique identifier of the target user that can use the prepared message */
  Integer user_id;
  /* A JSON-serialized object describing the message to be sent */
  box<InlineQueryResult> result;
  /* Pass True if the message can be sent to private chats with users */
  optional<bool> allow_user_chats;
  /* Pass True if the message can be sent to private chats with bots */
  optional<bool> allow_bot_chats;
  /* Pass True if the message can be sent to group and supergroup chats */
  optional<bool> allow_group_chats;
  /* Pass True if the message can be sent to channel chats */
  optional<bool> allow_channel_chats;

  using return_type = PreparedInlineMessage;
  static constexpr file_info_e file_info = file_info_e::no;
  static constexpr std::string_view api_method_name = "savePreparedInlineMessage";
  static constexpr http_method_e http_method = http_method_e::POST;

  void fill_nonfile_args(auto& body) const {
    body.arg("user_id", user_id);
    body.arg("result", result);
    if (allow_user_chats)
      body.arg("allow_user_chats", *allow_user_chats);
    if (allow_bot_chats)
      body.arg("allow_bot_chats", *allow_bot_chats);
    if (allow_group_chats)
      body.arg("allow_group_chats", *allow_group_chats);
    if (allow_channel_chats)
      body.arg("allow_channel_chats", *allow_channel_chats);
  }
};

}  // namespace tgbm::api
