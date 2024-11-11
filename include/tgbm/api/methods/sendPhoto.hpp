#pragma once

#include "tgbm/api/common.hpp"
#include "tgbm/api/types/Message.hpp"
#include "tgbm/api/types/MessageEntity.hpp"
#include "tgbm/api/types/ReplyParameters.hpp"

namespace tgbm::api {

struct send_photo_request {
  /* Unique identifier for the target chat or username of the target channel (in the format @channelusername)
   */
  int_or_str chat_id;
  /* Photo to send. Pass a file_id as String to send a photo that exists on the Telegram servers
   * (recommended), pass an HTTP URL as a String for Telegram to get a photo from the Internet, or upload a
   * new photo using multipart/form-data. The photo must be at most 10 MB in size. The photo's width and
   * height must not exceed 10000 in total. Width and height ratio must be at most 20. More information on
   * Sending Files » */
  file_or_str photo;
  /* Unique identifier of the business connection on behalf of which the message will be sent */
  optional<String> business_connection_id;
  /* Unique identifier for the target message thread (topic) of the forum; for forum supergroups only */
  optional<Integer> message_thread_id;
  /* Photo caption (may also be used when resending photos by file_id), 0-1024 characters after entities
   * parsing */
  optional<String> caption;
  /* Mode for parsing entities in the photo caption. See formatting options for more details. */
  optional<String> parse_mode;
  /* A JSON-serialized list of special entities that appear in the caption, which can be specified instead of
   * parse_mode */
  optional<arrayof<MessageEntity>> caption_entities;
  /* Pass True, if the caption must be shown above the message media */
  optional<bool> show_caption_above_media;
  /* Pass True if the photo needs to be covered with a spoiler animation */
  optional<bool> has_spoiler;
  /* Sends the message silently. Users will receive a notification with no sound. */
  optional<bool> disable_notification;
  /* Protects the contents of the sent message from forwarding and saving */
  optional<bool> protect_content;
  /* Pass True to allow up to 1000 messages per second, ignoring broadcasting limits for a fee of 0.1 Telegram
   * Stars per message. The relevant Stars will be withdrawn from the bot's balance */
  optional<bool> allow_paid_broadcast;
  /* Unique identifier of the message effect to be added to the message; for private chats only */
  optional<String> message_effect_id;
  /* Description of the message to reply to */
  box<ReplyParameters> reply_parameters;
  /* Additional interface options. A JSON-serialized object for an inline keyboard, custom reply keyboard,
   * instructions to remove a reply keyboard or to force a reply from the user */
  optional<reply_markup_t> reply_markup;

  using return_type = Message;
  static constexpr file_info_e file_info = file_info_e::maybe;
  static constexpr std::string_view api_method_name = "sendPhoto";
  static constexpr http_method_e http_method = http_method_e::POST;

  void fill_nonfile_args(auto& body) const {
    if (business_connection_id)
      body.arg("business_connection_id", *business_connection_id);
    body.arg("chat_id", chat_id);
    if (message_thread_id)
      body.arg("message_thread_id", *message_thread_id);
    if (const auto* str = photo.get_str())
      body.arg("photo", *str);
    if (caption)
      body.arg("caption", *caption);
    if (parse_mode)
      body.arg("parse_mode", *parse_mode);
    if (caption_entities)
      body.arg("caption_entities", *caption_entities);
    if (show_caption_above_media)
      body.arg("show_caption_above_media", *show_caption_above_media);
    if (has_spoiler)
      body.arg("has_spoiler", *has_spoiler);
    if (disable_notification)
      body.arg("disable_notification", *disable_notification);
    if (protect_content)
      body.arg("protect_content", *protect_content);
    if (allow_paid_broadcast)
      body.arg("allow_paid_broadcast", *allow_paid_broadcast);
    if (message_effect_id)
      body.arg("message_effect_id", *message_effect_id);
    if (reply_parameters)
      body.arg("reply_parameters", *reply_parameters);
    if (reply_markup)
      body.arg("reply_markup", *reply_markup);
  }

  [[nodiscard]] bool has_file_args() const noexcept {
    if (photo.is_file())
      return true;
    return false;
  }

  void fill_file_args(application_multipart_form_data& body) const {
    if (const InputFile* f = photo.get_file())
      body.arg("photo", *f);
  }
};

}  // namespace tgbm::api
