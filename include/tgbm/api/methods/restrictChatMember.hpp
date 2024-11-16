#pragma once

#include <tgbm/api/common.hpp>
#include <tgbm/api/types/ChatPermissions.hpp>

namespace tgbm::api {

struct restrict_chat_member_request {
  /* Unique identifier for the target chat or username of the target supergroup (in the format
   * @supergroupusername) */
  int_or_str chat_id;
  /* Unique identifier of the target user */
  Integer user_id;
  /* A JSON-serialized object for new user permissions */
  box<ChatPermissions> permissions;
  /* Pass True if chat permissions are set independently. Otherwise, the can_send_other_messages and
   * can_add_web_page_previews permissions will imply the can_send_messages, can_send_audios,
   * can_send_documents, can_send_photos, can_send_videos, can_send_video_notes, and can_send_voice_notes
   * permissions; the can_send_polls permission will imply the can_send_messages permission. */
  optional<bool> use_independent_chat_permissions;
  /* Date when restrictions will be lifted for the user; Unix time. If user is restricted for more than 366
   * days or less than 30 seconds from the current time, they are considered to be restricted forever */
  optional<Integer> until_date;

  using return_type = bool;
  static constexpr file_info_e file_info = file_info_e::no;
  static constexpr std::string_view api_method_name = "restrictChatMember";
  static constexpr http_method_e http_method = http_method_e::POST;

  void fill_nonfile_args(auto& body) const {
    body.arg("chat_id", chat_id);
    body.arg("user_id", user_id);
    body.arg("permissions", permissions);
    if (use_independent_chat_permissions)
      body.arg("use_independent_chat_permissions", *use_independent_chat_permissions);
    if (until_date)
      body.arg("until_date", *until_date);
  }
};

}  // namespace tgbm::api
