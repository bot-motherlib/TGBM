#pragma once

#include "all_fwd.hpp"

namespace tgbm::api {

/*Describes actions that a non-administrator user is allowed to take in a chat.*/
struct ChatPermissions {
  /* Optional. True, if the user is allowed to send text messages, contacts, giveaways, giveaway winners,
   * invoices, locations and venues */
  optional<bool> can_send_messages;
  /* Optional. True, if the user is allowed to send audios */
  optional<bool> can_send_audios;
  /* Optional. True, if the user is allowed to send documents */
  optional<bool> can_send_documents;
  /* Optional. True, if the user is allowed to send photos */
  optional<bool> can_send_photos;
  /* Optional. True, if the user is allowed to send videos */
  optional<bool> can_send_videos;
  /* Optional. True, if the user is allowed to send video notes */
  optional<bool> can_send_video_notes;
  /* Optional. True, if the user is allowed to send voice notes */
  optional<bool> can_send_voice_notes;
  /* Optional. True, if the user is allowed to send polls */
  optional<bool> can_send_polls;
  /* Optional. True, if the user is allowed to send animations, games, stickers and use inline bots */
  optional<bool> can_send_other_messages;
  /* Optional. True, if the user is allowed to add web page previews to their messages */
  optional<bool> can_add_web_page_previews;
  /* Optional. True, if the user is allowed to change the chat title, photo and other settings. Ignored in
   * public supergroups */
  optional<bool> can_change_info;
  /* Optional. True, if the user is allowed to invite new users to the chat */
  optional<bool> can_invite_users;
  /* Optional. True, if the user is allowed to pin messages. Ignored in public supergroups */
  optional<bool> can_pin_messages;
  /* Optional. True, if the user is allowed to create forum topics. If omitted defaults to the value of
   * can_pin_messages */
  optional<bool> can_manage_topics;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).or_default(false);
  }
};

}  // namespace tgbm::api
