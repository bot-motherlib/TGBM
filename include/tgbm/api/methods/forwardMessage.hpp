#pragma once

#include <tgbm/api/common.hpp>
#include <tgbm/api/types/Message.hpp>

namespace tgbm::api {

struct forward_message_request {
  /* Unique identifier for the target chat or username of the target channel (in the format @channelusername)
   */
  int_or_str chat_id;
  /* Unique identifier for the chat where the original message was sent (or channel username in the format
   * @channelusername) */
  int_or_str from_chat_id;
  /* Message identifier in the chat specified in from_chat_id */
  Integer message_id;
  /* Unique identifier for the target message thread (topic) of the forum; for forum supergroups only */
  optional<Integer> message_thread_id;
  /* New start timestamp for the forwarded video in the message */
  optional<Integer> video_start_timestamp;
  /* Sends the message silently. Users will receive a notification with no sound. */
  optional<bool> disable_notification;
  /* Protects the contents of the forwarded message from forwarding and saving */
  optional<bool> protect_content;

  using return_type = Message;
  static constexpr file_info_e file_info = file_info_e::no;
  static constexpr std::string_view api_method_name = "forwardMessage";
  static constexpr http_method_e http_method = http_method_e::POST;

  void fill_nonfile_args(auto& body) const {
    body.arg("chat_id", chat_id);
    if (message_thread_id)
      body.arg("message_thread_id", *message_thread_id);
    body.arg("from_chat_id", from_chat_id);
    if (video_start_timestamp)
      body.arg("video_start_timestamp", *video_start_timestamp);
    if (disable_notification)
      body.arg("disable_notification", *disable_notification);
    if (protect_content)
      body.arg("protect_content", *protect_content);
    body.arg("message_id", message_id);
  }
};

}  // namespace tgbm::api
