#pragma once

#include <tgbm/api/common.hpp>
#include <tgbm/api/types/MessageId.hpp>

namespace tgbm::api {

struct copy_messages_request {
  /* Unique identifier for the target chat or username of the target channel (in the format @channelusername)
   */
  int_or_str chat_id;
  /* Unique identifier for the chat where the original messages were sent (or channel username in the format
   * @channelusername) */
  int_or_str from_chat_id;
  /* A JSON-serialized list of 1-100 identifiers of messages in the chat from_chat_id to copy. The identifiers
   * must be specified in a strictly increasing order. */
  arrayof<Integer> message_ids;
  /* Unique identifier for the target message thread (topic) of the forum; for forum supergroups only */
  optional<Integer> message_thread_id;
  /* Sends the messages silently. Users will receive a notification with no sound. */
  optional<bool> disable_notification;
  /* Protects the contents of the sent messages from forwarding and saving */
  optional<bool> protect_content;
  /* Pass True to copy the messages without their captions */
  optional<bool> remove_caption;

  using return_type = arrayof<MessageId>;
  static constexpr file_info_e file_info = file_info_e::no;
  static constexpr std::string_view api_method_name = "copyMessages";
  static constexpr http_method_e http_method = http_method_e::POST;

  void fill_nonfile_args(auto& body) const {
    body.arg("chat_id", chat_id);
    if (message_thread_id)
      body.arg("message_thread_id", *message_thread_id);
    body.arg("from_chat_id", from_chat_id);
    body.arg("message_ids", message_ids);
    if (disable_notification)
      body.arg("disable_notification", *disable_notification);
    if (protect_content)
      body.arg("protect_content", *protect_content);
    if (remove_caption)
      body.arg("remove_caption", *remove_caption);
  }
};

}  // namespace tgbm::api
