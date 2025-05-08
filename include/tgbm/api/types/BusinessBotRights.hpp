#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*Represents the rights of a business bot.*/
struct BusinessBotRights {
  /* Optional. True, if the bot can send and edit messages in the private chats that had incoming messages in
   * the last 24 hours */
  optional<True> can_reply;
  /* Optional. True, if the bot can mark incoming private messages as read */
  optional<True> can_read_messages;
  /* Optional. True, if the bot can delete messages sent by the bot */
  optional<True> can_delete_sent_messages;
  /* Optional. True, if the bot can delete all private messages in managed chats */
  optional<True> can_delete_all_messages;
  /* Optional. True, if the bot can edit the first and last name of the business account */
  optional<True> can_edit_name;
  /* Optional. True, if the bot can edit the bio of the business account */
  optional<True> can_edit_bio;
  /* Optional. True, if the bot can edit the profile photo of the business account */
  optional<True> can_edit_profile_photo;
  /* Optional. True, if the bot can edit the username of the business account */
  optional<True> can_edit_username;
  /* Optional. True, if the bot can change the privacy settings pertaining to gifts for the business account
   */
  optional<True> can_change_gift_settings;
  /* Optional. True, if the bot can view gifts and the amount of Telegram Stars owned by the business account
   */
  optional<True> can_view_gifts_and_stars;
  /* Optional. True, if the bot can convert regular gifts owned by the business account to Telegram Stars */
  optional<True> can_convert_gifts_to_stars;
  /* Optional. True, if the bot can transfer and upgrade gifts owned by the business account */
  optional<True> can_transfer_and_upgrade_gifts;
  /* Optional. True, if the bot can transfer Telegram Stars received by the business account to its own
   * account, or use them to upgrade and transfer gifts */
  optional<True> can_transfer_stars;
  /* Optional. True, if the bot can post, edit and delete stories on behalf of the business account */
  optional<True> can_manage_stories;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name).or_default(false);
  }
};

}  // namespace tgbm::api
