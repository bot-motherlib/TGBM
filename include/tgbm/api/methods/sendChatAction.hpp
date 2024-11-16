#pragma once

#include <tgbm/api/common.hpp>

namespace tgbm::api {

struct send_chat_action_request {
  /* Unique identifier for the target chat or username of the target channel (in the format @channelusername)
   */
  int_or_str chat_id;
  /* Type of action to broadcast. Choose one, depending on what the user is about to receive: typing for text
   * messages, upload_photo for photos, record_video or upload_video for videos, record_voice or upload_voice
   * for voice notes, upload_document for general files, choose_sticker for stickers, find_location for
   * location data, record_video_note or upload_video_note for video notes. */
  String action;
  /* Unique identifier of the business connection on behalf of which the action will be sent */
  optional<String> business_connection_id;
  /* Unique identifier for the target message thread; for supergroups only */
  optional<Integer> message_thread_id;

  using return_type = bool;
  static constexpr file_info_e file_info = file_info_e::no;
  static constexpr std::string_view api_method_name = "sendChatAction";
  static constexpr http_method_e http_method = http_method_e::POST;

  void fill_nonfile_args(auto& body) const {
    if (business_connection_id)
      body.arg("business_connection_id", *business_connection_id);
    body.arg("chat_id", chat_id);
    if (message_thread_id)
      body.arg("message_thread_id", *message_thread_id);
    body.arg("action", action);
  }
};

}  // namespace tgbm::api
