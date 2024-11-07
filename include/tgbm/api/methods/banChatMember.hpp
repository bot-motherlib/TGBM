#pragma once

#include "tgbm/api/common.hpp"

namespace tgbm::api {

struct ban_chat_member_request {
  /* Unique identifier for the target group or username of the target supergroup or channel (in the format
   * @channelusername) */
  int_or_str chat_id;
  /* Unique identifier of the target user */
  Integer user_id;
  /* Date when the user will be unbanned; Unix time. If user is banned for more than 366 days or less than 30
   * seconds from the current time they are considered to be banned forever. Applied for supergroups and
   * channels only. */
  optional<Integer> until_date;
  /* Pass True to delete all messages from the chat for the user that is being removed. If False, the user
   * will be able to see messages in the group that were sent before the user was removed. Always True for
   * supergroups and channels. */
  optional<bool> revoke_messages;

  using return_type = bool;
  static constexpr file_info_e file_info = file_info_e::no;
  static constexpr std::string_view api_method_name = "banChatMember";
  static constexpr http_method_e http_method = http_method_e::POST;

  void fill_nonfile_args(auto& body) const {
    body.arg("chat_id", chat_id);
    body.arg("user_id", user_id);
    if (until_date)
      body.arg("until_date", *until_date);
    if (revoke_messages)
      body.arg("revoke_messages", *revoke_messages);
  }
};

}  // namespace tgbm::api
