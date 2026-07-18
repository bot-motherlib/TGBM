#pragma once

#include <tgbm/api/common.hpp>
#include <tgbm/api/types/InlineKeyboardMarkup.hpp>
#include <tgbm/api/types/InputMedia.hpp>

namespace tgbm::api {

struct edit_ephemeral_message_media_request {
  /* Unique identifier for the target chat or username of the target supergroup in the format @username */
  int_or_str chat_id;
  /* Identifier of the user who received the message */
  Integer receiver_user_id;
  /* Identifier of the ephemeral message to edit */
  Integer ephemeral_message_id;
  /* A JSON-serialized object for the new media content of the message. A new file can't be uploaded; use a
   * previously uploaded file via its file_id or specify a URL. */
  box<InputMedia> media;
  /* A JSON-serialized object for an inline keyboard */
  box<InlineKeyboardMarkup> reply_markup;

  using return_type = bool;
  static constexpr file_info_e file_info = file_info_e::no;
  static constexpr std::string_view api_method_name = "editEphemeralMessageMedia";
  static constexpr http_method_e http_method = http_method_e::POST;

  void fill_nonfile_args(auto& body) const {
    body.arg("chat_id", chat_id);
    body.arg("receiver_user_id", receiver_user_id);
    body.arg("ephemeral_message_id", ephemeral_message_id);
    body.arg("media", media);
    if (reply_markup)
      body.arg("reply_markup", *reply_markup);
  }
};

}  // namespace tgbm::api
