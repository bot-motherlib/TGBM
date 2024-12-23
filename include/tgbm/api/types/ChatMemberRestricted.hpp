#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*This object contains information about one member of a chat. Currently, the following 6 types of chat
 * members are supported:*/
struct ChatMemberRestricted {
  /* Information about the user */
  box<User> user;
  /* True, if the user is a member of the chat at the moment of the request */
  bool is_member;
  /* True, if the user is allowed to send text messages, contacts, giveaways, giveaway winners, invoices,
   * locations and venues */
  bool can_send_messages;
  /* True, if the user is allowed to send audios */
  bool can_send_audios;
  /* True, if the user is allowed to send documents */
  bool can_send_documents;
  /* True, if the user is allowed to send photos */
  bool can_send_photos;
  /* True, if the user is allowed to send videos */
  bool can_send_videos;
  /* True, if the user is allowed to send video notes */
  bool can_send_video_notes;
  /* True, if the user is allowed to send voice notes */
  bool can_send_voice_notes;
  /* True, if the user is allowed to send polls */
  bool can_send_polls;
  /* True, if the user is allowed to send animations, games, stickers and use inline bots */
  bool can_send_other_messages;
  /* True, if the user is allowed to add web page previews to their messages */
  bool can_add_web_page_previews;
  /* True, if the user is allowed to change the chat title, photo and other settings */
  bool can_change_info;
  /* True, if the user is allowed to invite new users to the chat */
  bool can_invite_users;
  /* True, if the user is allowed to pin messages */
  bool can_pin_messages;
  /* True, if the user is allowed to create forum topics */
  bool can_manage_topics;
  /* Date when restrictions will be lifted for this user; Unix time. If 0, then the user is restricted forever
   */
  Integer until_date;

  constexpr static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name)
        .case_("user", true)
        .case_("is_member", true)
        .case_("can_send_messages", true)
        .case_("can_send_audios", true)
        .case_("can_send_documents", true)
        .case_("can_send_photos", true)
        .case_("can_send_videos", true)
        .case_("can_send_video_notes", true)
        .case_("can_send_voice_notes", true)
        .case_("can_send_polls", true)
        .case_("can_send_other_messages", true)
        .case_("can_add_web_page_previews", true)
        .case_("can_change_info", true)
        .case_("can_invite_users", true)
        .case_("can_pin_messages", true)
        .case_("can_manage_topics", true)
        .case_("until_date", true)
        .or_default(false);
  }
};

}  // namespace tgbm::api
