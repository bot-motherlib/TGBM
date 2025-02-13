#pragma once

#include <tgbm/api/common.hpp>
#include <tgbm/api/types/MessageEntity.hpp>
#include <tgbm/api/types/MessageId.hpp>
#include <tgbm/api/types/ReplyParameters.hpp>

namespace tgbm::api {

struct copy_message_request {
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
  /* New start timestamp for the copied video in the message */
  optional<Integer> video_start_timestamp;
  /* New caption for media, 0-1024 characters after entities parsing. If not specified, the original caption
   * is kept */
  optional<String> caption;
  /* Mode for parsing entities in the new caption. See formatting options for more details. */
  optional<String> parse_mode;
  /* A JSON-serialized list of special entities that appear in the new caption, which can be specified instead
   * of parse_mode */
  optional<arrayof<MessageEntity>> caption_entities;
  /* Pass True, if the caption must be shown above the message media. Ignored if a new caption isn't
   * specified. */
  optional<bool> show_caption_above_media;
  /* Sends the message silently. Users will receive a notification with no sound. */
  optional<bool> disable_notification;
  /* Protects the contents of the sent message from forwarding and saving */
  optional<bool> protect_content;
  /* Pass True to allow up to 1000 messages per second, ignoring broadcasting limits for a fee of 0.1 Telegram
   * Stars per message. The relevant Stars will be withdrawn from the bot's balance */
  optional<bool> allow_paid_broadcast;
  /* Description of the message to reply to */
  box<ReplyParameters> reply_parameters;
  /* Additional interface options. A JSON-serialized object for an inline keyboard, custom reply keyboard,
   * instructions to remove a reply keyboard or to force a reply from the user */
  optional<reply_markup_t> reply_markup;

  using return_type = MessageId;
  static constexpr file_info_e file_info = file_info_e::no;
  static constexpr std::string_view api_method_name = "copyMessage";
  static constexpr http_method_e http_method = http_method_e::POST;

  void fill_nonfile_args(auto& body) const {
    body.arg("chat_id", chat_id);
    if (message_thread_id)
      body.arg("message_thread_id", *message_thread_id);
    body.arg("from_chat_id", from_chat_id);
    body.arg("message_id", message_id);
    if (video_start_timestamp)
      body.arg("video_start_timestamp", *video_start_timestamp);
    if (caption)
      body.arg("caption", *caption);
    if (parse_mode)
      body.arg("parse_mode", *parse_mode);
    if (caption_entities)
      body.arg("caption_entities", *caption_entities);
    if (show_caption_above_media)
      body.arg("show_caption_above_media", *show_caption_above_media);
    if (disable_notification)
      body.arg("disable_notification", *disable_notification);
    if (protect_content)
      body.arg("protect_content", *protect_content);
    if (allow_paid_broadcast)
      body.arg("allow_paid_broadcast", *allow_paid_broadcast);
    if (reply_parameters)
      body.arg("reply_parameters", *reply_parameters);
    if (reply_markup)
      body.arg("reply_markup", *reply_markup);
  }
};

}  // namespace tgbm::api
