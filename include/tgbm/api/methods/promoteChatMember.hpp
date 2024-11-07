#pragma once

#include "tgbm/api/common.hpp"

namespace tgbm::api {

struct promote_chat_member_request {
  /* Unique identifier for the target chat or username of the target channel (in the format @channelusername)
   */
  int_or_str chat_id;
  /* Unique identifier of the target user */
  Integer user_id;
  /* Pass True if the administrator's presence in the chat is hidden */
  optional<bool> is_anonymous;
  /* Pass True if the administrator can access the chat event log, get boost list, see hidden supergroup and
   * channel members, report spam messages and ignore slow mode. Implied by any other administrator privilege.
   */
  optional<bool> can_manage_chat;
  /* Pass True if the administrator can delete messages of other users */
  optional<bool> can_delete_messages;
  /* Pass True if the administrator can manage video chats */
  optional<bool> can_manage_video_chats;
  /* Pass True if the administrator can restrict, ban or unban chat members, or access supergroup statistics
   */
  optional<bool> can_restrict_members;
  /* Pass True if the administrator can add new administrators with a subset of their own privileges or demote
   * administrators that they have promoted, directly or indirectly (promoted by administrators that were
   * appointed by him) */
  optional<bool> can_promote_members;
  /* Pass True if the administrator can change chat title, photo and other settings */
  optional<bool> can_change_info;
  /* Pass True if the administrator can invite new users to the chat */
  optional<bool> can_invite_users;
  /* Pass True if the administrator can post stories to the chat */
  optional<bool> can_post_stories;
  /* Pass True if the administrator can edit stories posted by other users, post stories to the chat page, pin
   * chat stories, and access the chat's story archive */
  optional<bool> can_edit_stories;
  /* Pass True if the administrator can delete stories posted by other users */
  optional<bool> can_delete_stories;
  /* Pass True if the administrator can post messages in the channel, or access channel statistics; for
   * channels only */
  optional<bool> can_post_messages;
  /* Pass True if the administrator can edit messages of other users and can pin messages; for channels only
   */
  optional<bool> can_edit_messages;
  /* Pass True if the administrator can pin messages; for supergroups only */
  optional<bool> can_pin_messages;
  /* Pass True if the user is allowed to create, rename, close, and reopen forum topics; for supergroups only
   */
  optional<bool> can_manage_topics;

  using return_type = bool;
  static constexpr file_info_e file_info = file_info_e::no;
  static constexpr std::string_view api_method_name = "promoteChatMember";
  static constexpr http_method_e http_method = http_method_e::POST;

  void fill_nonfile_args(auto& body) const {
    body.arg("chat_id", chat_id);
    body.arg("user_id", user_id);
    if (is_anonymous)
      body.arg("is_anonymous", *is_anonymous);
    if (can_manage_chat)
      body.arg("can_manage_chat", *can_manage_chat);
    if (can_delete_messages)
      body.arg("can_delete_messages", *can_delete_messages);
    if (can_manage_video_chats)
      body.arg("can_manage_video_chats", *can_manage_video_chats);
    if (can_restrict_members)
      body.arg("can_restrict_members", *can_restrict_members);
    if (can_promote_members)
      body.arg("can_promote_members", *can_promote_members);
    if (can_change_info)
      body.arg("can_change_info", *can_change_info);
    if (can_invite_users)
      body.arg("can_invite_users", *can_invite_users);
    if (can_post_stories)
      body.arg("can_post_stories", *can_post_stories);
    if (can_edit_stories)
      body.arg("can_edit_stories", *can_edit_stories);
    if (can_delete_stories)
      body.arg("can_delete_stories", *can_delete_stories);
    if (can_post_messages)
      body.arg("can_post_messages", *can_post_messages);
    if (can_edit_messages)
      body.arg("can_edit_messages", *can_edit_messages);
    if (can_pin_messages)
      body.arg("can_pin_messages", *can_pin_messages);
    if (can_manage_topics)
      body.arg("can_manage_topics", *can_manage_topics);
  }
};

}  // namespace tgbm::api
