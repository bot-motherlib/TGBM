#pragma once

#include "tgbm/api/common.hpp"
#include "tgbm/api/types/Message.hpp"
#include "tgbm/api/types/MessageEntity.hpp"
#include "tgbm/api/types/ReplyParameters.hpp"

namespace tgbm::api {

struct send_animation_request {
  /* Unique identifier for the target chat or username of the target channel (in the format @channelusername)
   */
  int_or_str chat_id;
  /* Animation to send. Pass a file_id as String to send an animation that exists on the Telegram servers
   * (recommended), pass an HTTP URL as a String for Telegram to get an animation from the Internet, or upload
   * a new animation using multipart/form-data. More information on Sending Files » */
  file_or_str animation;
  /* Unique identifier of the business connection on behalf of which the message will be sent */
  optional<String> business_connection_id;
  /* Unique identifier for the target message thread (topic) of the forum; for forum supergroups only */
  optional<Integer> message_thread_id;
  /* Duration of sent animation in seconds */
  optional<Integer> duration;
  /* Animation width */
  optional<Integer> width;
  /* Animation height */
  optional<Integer> height;
  /* Thumbnail of the file sent; can be ignored if thumbnail generation for the file is supported server-side.
   * The thumbnail should be in JPEG format and less than 200 kB in size. A thumbnail's width and height
   * should not exceed 320. Ignored if the file is not uploaded using multipart/form-data. Thumbnails can't be
   * reused and can be only uploaded as a new file, so you can pass “attach://<file_attach_name>” if the
   * thumbnail was uploaded using multipart/form-data under <file_attach_name>. More information on Sending
   * Files » */
  optional<file_or_str> thumbnail;
  /* Animation caption (may also be used when resending animation by file_id), 0-1024 characters after
   * entities parsing */
  optional<String> caption;
  /* Mode for parsing entities in the animation caption. See formatting options for more details. */
  optional<String> parse_mode;
  /* A JSON-serialized list of special entities that appear in the caption, which can be specified instead of
   * parse_mode */
  optional<arrayof<MessageEntity>> caption_entities;
  /* Pass True, if the caption must be shown above the message media */
  optional<bool> show_caption_above_media;
  /* Pass True if the animation needs to be covered with a spoiler animation */
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
  static constexpr std::string_view api_method_name = "sendAnimation";
  static constexpr http_method_e http_method = http_method_e::POST;

  void fill_nonfile_args(auto& body) const {
    if (business_connection_id)
      body.arg("business_connection_id", *business_connection_id);
    body.arg("chat_id", chat_id);
    if (message_thread_id)
      body.arg("message_thread_id", *message_thread_id);
    if (const auto* str = animation.get_str())
      body.arg("animation", *str);
    if (duration)
      body.arg("duration", *duration);
    if (width)
      body.arg("width", *width);
    if (height)
      body.arg("height", *height);
    if (thumbnail)
      if (const auto* str = thumbnail->get_str())
        body.arg("thumbnail", *str);
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
    if (animation.is_file())
      return true;
    if (thumbnail && thumbnail->is_file())
      return true;
    return false;
  }

  void fill_file_args(application_multipart_form_data& body) const {
    if (const InputFile* f = animation.get_file())
      body.arg("animation", *f);
    if (thumbnail)
      if (const InputFile* f = thumbnail->get_file())
        body.arg("thumbnail", *f);
  }
};

}  // namespace tgbm::api
