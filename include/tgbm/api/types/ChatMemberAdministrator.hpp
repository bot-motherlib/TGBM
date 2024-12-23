#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*This object contains information about one member of a chat. Currently, the following 6 types of chat
 * members are supported:*/
struct ChatMemberAdministrator {
  /* Information about the user */
  box<User> user;
  /* True, if the bot is allowed to edit administrator privileges of that user */
  bool can_be_edited;
  /* True, if the user's presence in the chat is hidden */
  bool is_anonymous;
  /* True, if the administrator can access the chat event log, get boost list, see hidden supergroup and
   * channel members, report spam messages and ignore slow mode. Implied by any other administrator privilege.
   */
  bool can_manage_chat;
  /* True, if the administrator can delete messages of other users */
  bool can_delete_messages;
  /* True, if the administrator can manage video chats */
  bool can_manage_video_chats;
  /* True, if the administrator can restrict, ban or unban chat members, or access supergroup statistics */
  bool can_restrict_members;
  /* True, if the administrator can add new administrators with a subset of their own privileges or demote
   * administrators that they have promoted, directly or indirectly (promoted by administrators that were
   * appointed by the user) */
  bool can_promote_members;
  /* True, if the user is allowed to change the chat title, photo and other settings */
  bool can_change_info;
  /* True, if the user is allowed to invite new users to the chat */
  bool can_invite_users;
  /* True, if the administrator can post stories to the chat */
  bool can_post_stories;
  /* True, if the administrator can edit stories posted by other users, post stories to the chat page, pin
   * chat stories, and access the chat's story archive */
  bool can_edit_stories;
  /* True, if the administrator can delete stories posted by other users */
  bool can_delete_stories;
  /* Optional. Custom title for this user */
  optional<String> custom_title;
  /* Optional. True, if the administrator can post messages in the channel, or access channel statistics; for
   * channels only */
  optional<bool> can_post_messages;
  /* Optional. True, if the administrator can edit messages of other users and can pin messages; for channels
   * only */
  optional<bool> can_edit_messages;
  /* Optional. True, if the user is allowed to pin messages; for groups and supergroups only */
  optional<bool> can_pin_messages;
  /* Optional. True, if the user is allowed to create, rename, close, and reopen forum topics; for supergroups
   * only */
  optional<bool> can_manage_topics;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name)
        .case_("user", true)
        .case_("can_be_edited", true)
        .case_("is_anonymous", true)
        .case_("can_manage_chat", true)
        .case_("can_delete_messages", true)
        .case_("can_manage_video_chats", true)
        .case_("can_restrict_members", true)
        .case_("can_promote_members", true)
        .case_("can_change_info", true)
        .case_("can_invite_users", true)
        .case_("can_post_stories", true)
        .case_("can_edit_stories", true)
        .case_("can_delete_stories", true)
        .or_default(false);
  }
};

}  // namespace tgbm::api
