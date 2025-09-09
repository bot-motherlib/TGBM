#pragma once

#include <tgbm/api/common.hpp>
#include <tgbm/api/types/Message.hpp>
#include <tgbm/api/types/ReplyParameters.hpp>
#include <tgbm/api/types/SuggestedPostParameters.hpp>

namespace tgbm::api {

struct send_video_note_request {
  /* Unique identifier for the target chat or username of the target channel (in the format @channelusername)
   */
  int_or_str chat_id;
  /* Video note to send. Pass a file_id as String to send a video note that exists on the Telegram servers
   * (recommended) or upload a new video using multipart/form-data. More information on Sending Files ».
   * Sending video notes by a URL is currently unsupported */
  file_or_str video_note;
  /* Unique identifier of the business connection on behalf of which the message will be sent */
  optional<String> business_connection_id;
  /* Unique identifier for the target message thread (topic) of the forum; for forum supergroups only */
  optional<Integer> message_thread_id;
  /* Identifier of the direct messages topic to which the message will be sent; required if the message is
   * sent to a direct messages chat */
  optional<Integer> direct_messages_topic_id;
  /* Duration of sent video in seconds */
  optional<Integer> duration;
  /* Video width and height, i.e. diameter of the video message */
  optional<Integer> length;
  /* Thumbnail of the file sent; can be ignored if thumbnail generation for the file is supported server-side.
   * The thumbnail should be in JPEG format and less than 200 kB in size. A thumbnail's width and height
   * should not exceed 320. Ignored if the file is not uploaded using multipart/form-data. Thumbnails can't be
   * reused and can be only uploaded as a new file, so you can pass “attach://<file_attach_name>” if the
   * thumbnail was uploaded using multipart/form-data under <file_attach_name>. More information on Sending
   * Files » */
  optional<file_or_str> thumbnail;
  /* Sends the message silently. Users will receive a notification with no sound. */
  optional<bool> disable_notification;
  /* Protects the contents of the sent message from forwarding and saving */
  optional<bool> protect_content;
  /* Pass True to allow up to 1000 messages per second, ignoring broadcasting limits for a fee of 0.1 Telegram
   * Stars per message. The relevant Stars will be withdrawn from the bot's balance */
  optional<bool> allow_paid_broadcast;
  /* Unique identifier of the message effect to be added to the message; for private chats only */
  optional<String> message_effect_id;
  /* A JSON-serialized object containing the parameters of the suggested post to send; for direct messages
   * chats only. If the message is sent as a reply to another suggested post, then that suggested post is
   * automatically declined. */
  box<SuggestedPostParameters> suggested_post_parameters;
  /* Description of the message to reply to */
  box<ReplyParameters> reply_parameters;
  /* Additional interface options. A JSON-serialized object for an inline keyboard, custom reply keyboard,
   * instructions to remove a reply keyboard or to force a reply from the user */
  optional<reply_markup_t> reply_markup;

  using return_type = Message;
  static constexpr file_info_e file_info = file_info_e::maybe;
  static constexpr std::string_view api_method_name = "sendVideoNote";
  static constexpr http_method_e http_method = http_method_e::POST;

  void fill_nonfile_args(auto& body) const {
    if (business_connection_id)
      body.arg("business_connection_id", *business_connection_id);
    body.arg("chat_id", chat_id);
    if (message_thread_id)
      body.arg("message_thread_id", *message_thread_id);
    if (direct_messages_topic_id)
      body.arg("direct_messages_topic_id", *direct_messages_topic_id);
    if (const auto* str = video_note.get_str())
      body.arg("video_note", *str);
    if (duration)
      body.arg("duration", *duration);
    if (length)
      body.arg("length", *length);
    if (thumbnail)
      if (const auto* str = thumbnail->get_str())
        body.arg("thumbnail", *str);
    if (disable_notification)
      body.arg("disable_notification", *disable_notification);
    if (protect_content)
      body.arg("protect_content", *protect_content);
    if (allow_paid_broadcast)
      body.arg("allow_paid_broadcast", *allow_paid_broadcast);
    if (message_effect_id)
      body.arg("message_effect_id", *message_effect_id);
    if (suggested_post_parameters)
      body.arg("suggested_post_parameters", *suggested_post_parameters);
    if (reply_parameters)
      body.arg("reply_parameters", *reply_parameters);
    if (reply_markup)
      body.arg("reply_markup", *reply_markup);
  }

  [[nodiscard]] bool has_file_args() const noexcept {
    if (video_note.is_file())
      return true;
    if (thumbnail && thumbnail->is_file())
      return true;
    return false;
  }

  void fill_file_args(application_multipart_form_data& body) const {
    if (const InputFile* f = video_note.get_file())
      body.arg("video_note", *f);
    if (thumbnail)
      if (const InputFile* f = thumbnail->get_file())
        body.arg("thumbnail", *f);
  }
};

}  // namespace tgbm::api
