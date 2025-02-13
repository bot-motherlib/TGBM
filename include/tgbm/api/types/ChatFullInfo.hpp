#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*This object contains full information about a chat.*/
struct ChatFullInfo {
  /* Unique identifier for this chat. This number may have more than 32 significant bits and some programming
   * languages may have difficulty/silent defects in interpreting it. But it has at most 52 significant bits,
   * so a signed 64-bit integer or double-precision float type are safe for storing this identifier. */
  Integer id;
  /* Type of the chat, can be either “private”, “group”, “supergroup” or “channel” */
  String type;
  /* Identifier of the accent color for the chat name and backgrounds of the chat photo, reply header, and
   * link preview. See accent colors for more details. */
  Integer accent_color_id;
  /* The maximum number of reactions that can be set on a message in the chat */
  Integer max_reaction_count;
  /* Optional. Title, for supergroups, channels and group chats */
  optional<String> title;
  /* Optional. Username, for private chats, supergroups and channels if available */
  optional<String> username;
  /* Optional. First name of the other party in a private chat */
  optional<String> first_name;
  /* Optional. Last name of the other party in a private chat */
  optional<String> last_name;
  /* Optional. Chat photo */
  box<ChatPhoto> photo;
  /* Optional. If non-empty, the list of all active chat usernames; for private chats, supergroups and
   * channels */
  optional<arrayof<String>> active_usernames;
  /* Optional. For private chats, the date of birth of the user */
  box<Birthdate> birthdate;
  /* Optional. For private chats with business accounts, the intro of the business */
  box<BusinessIntro> business_intro;
  /* Optional. For private chats with business accounts, the location of the business */
  box<BusinessLocation> business_location;
  /* Optional. For private chats with business accounts, the opening hours of the business */
  box<BusinessOpeningHours> business_opening_hours;
  /* Optional. For private chats, the personal channel of the user */
  box<Chat> personal_chat;
  /* Optional. List of available reactions allowed in the chat. If omitted, then all emoji reactions are
   * allowed. */
  optional<arrayof<ReactionType>> available_reactions;
  /* Optional. Custom emoji identifier of the emoji chosen by the chat for the reply header and link preview
   * background */
  optional<String> background_custom_emoji_id;
  /* Optional. Identifier of the accent color for the chat's profile background. See profile accent colors for
   * more details. */
  optional<Integer> profile_accent_color_id;
  /* Optional. Custom emoji identifier of the emoji chosen by the chat for its profile background */
  optional<String> profile_background_custom_emoji_id;
  /* Optional. Custom emoji identifier of the emoji status of the chat or the other party in a private chat */
  optional<String> emoji_status_custom_emoji_id;
  /* Optional. Expiration date of the emoji status of the chat or the other party in a private chat, in Unix
   * time, if any */
  optional<Integer> emoji_status_expiration_date;
  /* Optional. Bio of the other party in a private chat */
  optional<String> bio;
  /* Optional. Description, for groups, supergroups and channel chats */
  optional<String> description;
  /* Optional. Primary invite link, for groups, supergroups and channel chats */
  optional<String> invite_link;
  /* Optional. The most recent pinned message (by sending date) */
  box<Message> pinned_message;
  /* Optional. Default chat member permissions, for groups and supergroups */
  box<ChatPermissions> permissions;
  /* Optional. For supergroups, the minimum allowed delay between consecutive messages sent by each
   * unprivileged user; in seconds */
  optional<Integer> slow_mode_delay;
  /* Optional. For supergroups, the minimum number of boosts that a non-administrator user needs to add in
   * order to ignore slow mode and chat permissions */
  optional<Integer> unrestrict_boost_count;
  /* Optional. The time after which all messages sent to the chat will be automatically deleted; in seconds */
  optional<Integer> message_auto_delete_time;
  /* Optional. For supergroups, name of the group sticker set */
  optional<String> sticker_set_name;
  /* Optional. For supergroups, the name of the group's custom emoji sticker set. Custom emoji from this set
   * can be used by all users and bots in the group. */
  optional<String> custom_emoji_sticker_set_name;
  /* Optional. Unique identifier for the linked chat, i.e. the discussion group identifier for a channel and
   * vice versa; for supergroups and channel chats. This identifier may be greater than 32 bits and some
   * programming languages may have difficulty/silent defects in interpreting it. But it is smaller than 52
   * bits, so a signed 64 bit integer or double-precision float type are safe for storing this identifier. */
  optional<Integer> linked_chat_id;
  /* Optional. For supergroups, the location to which the supergroup is connected */
  box<ChatLocation> location;
  /* Optional. True, if the supergroup chat is a forum (has topics enabled) */
  optional<True> is_forum;
  /* Optional. True, if privacy settings of the other party in the private chat allows to use
   * tg://user?id=<user_id> links only in chats with the user */
  optional<True> has_private_forwards;
  /* Optional. True, if the privacy settings of the other party restrict sending voice and video note messages
   * in the private chat */
  optional<True> has_restricted_voice_and_video_messages;
  /* Optional. True, if users need to join the supergroup before they can send messages */
  optional<True> join_to_send_messages;
  /* Optional. True, if all users directly joining the supergroup without using an invite link need to be
   * approved by supergroup administrators */
  optional<True> join_by_request;
  /* Optional. True, if gifts can be sent to the chat */
  optional<True> can_send_gift;
  /* Optional. True, if paid media messages can be sent or forwarded to the channel chat. The field is
   * available only for channel chats. */
  optional<True> can_send_paid_media;
  /* Optional. True, if aggressive anti-spam checks are enabled in the supergroup. The field is only available
   * to chat administrators. */
  optional<True> has_aggressive_anti_spam_enabled;
  /* Optional. True, if non-administrators can only get the list of bots and administrators in the chat */
  optional<True> has_hidden_members;
  /* Optional. True, if messages from the chat can't be forwarded to other chats */
  optional<True> has_protected_content;
  /* Optional. True, if new chat members will have access to old messages; available only to chat
   * administrators */
  optional<True> has_visible_history;
  /* Optional. True, if the bot can change the group sticker set */
  optional<True> can_set_sticker_set;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name)
        .case_("id", true)
        .case_("type", true)
        .case_("accent_color_id", true)
        .case_("max_reaction_count", true)
        .or_default(false);
  }
};

}  // namespace tgbm::api
