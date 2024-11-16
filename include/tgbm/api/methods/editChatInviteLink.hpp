#pragma once

#include <tgbm/api/common.hpp>
#include <tgbm/api/types/ChatInviteLink.hpp>

namespace tgbm::api {

struct edit_chat_invite_link_request {
  /* Unique identifier for the target chat or username of the target channel (in the format @channelusername)
   */
  int_or_str chat_id;
  /* The invite link to edit */
  String invite_link;
  /* Invite link name; 0-32 characters */
  optional<String> name;
  /* Point in time (Unix timestamp) when the link will expire */
  optional<Integer> expire_date;
  /* The maximum number of users that can be members of the chat simultaneously after joining the chat via
   * this invite link; 1-99999 */
  optional<Integer> member_limit;
  /* True, if users joining the chat via the link need to be approved by chat administrators. If True,
   * member_limit can't be specified */
  optional<bool> creates_join_request;

  using return_type = ChatInviteLink;
  static constexpr file_info_e file_info = file_info_e::no;
  static constexpr std::string_view api_method_name = "editChatInviteLink";
  static constexpr http_method_e http_method = http_method_e::POST;

  void fill_nonfile_args(auto& body) const {
    body.arg("chat_id", chat_id);
    body.arg("invite_link", invite_link);
    if (name)
      body.arg("name", *name);
    if (expire_date)
      body.arg("expire_date", *expire_date);
    if (member_limit)
      body.arg("member_limit", *member_limit);
    if (creates_join_request)
      body.arg("creates_join_request", *creates_join_request);
  }
};

}  // namespace tgbm::api
