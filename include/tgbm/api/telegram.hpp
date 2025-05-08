#pragma once

#include <kelcoro/task.hpp>

#include <tgbm/net/http_client.hpp>
#include <tgbm/api/methods/all.hpp>

#include <tgbm/api/types/all.hpp>

namespace tgbm::api {

struct telegram {
  http_client& client;
  const_string bottoken;

  /* Use this method to receive incoming updates using long polling (wiki). Returns an Array of Update
   * objects. */
  dd::task<arrayof<Update>> getUpdates(api::get_updates_request,
                                       deadline_t deadline = deadline_t::never()) const;

  /* Use this method to specify a URL and receive incoming updates via an outgoing webhook. Whenever there is
   * an update for the bot, we will send an HTTPS POST request to the specified URL, containing a
   * JSON-serialized Update. In case of an unsuccessful request (a request with response HTTP status code
   * different from 2XY), we will repeat the request and give up after a reasonable amount of attempts.
   * Returns True on success. */
  dd::task<bool> setWebhook(api::set_webhook_request, deadline_t deadline = deadline_t::never()) const;

  /* Use this method to remove webhook integration if you decide to switch back to getUpdates. Returns True on
   * success. */
  dd::task<bool> deleteWebhook(api::delete_webhook_request, deadline_t deadline = deadline_t::never()) const;

  /* Use this method to get current webhook status. Requires no parameters. On success, returns a WebhookInfo
   * object. If the bot is using getUpdates, will return an object with the url field empty. */
  dd::task<WebhookInfo> getWebhookInfo(deadline_t deadline = deadline_t::never()) const;

  /* A simple method for testing your bot's authentication token. Requires no parameters. Returns basic
   * information about the bot in form of a User object. */
  dd::task<User> getMe(deadline_t deadline = deadline_t::never()) const;

  /* Use this method to log out from the cloud Bot API server before launching the bot locally. You must log
   * out the bot before running it locally, otherwise there is no guarantee that the bot will receive updates.
   * After a successful call, you can immediately log in on a local server, but will not be able to log in
   * back to the cloud Bot API server for 10 minutes. Returns True on success. Requires no parameters. */
  dd::task<bool> logOut(deadline_t deadline = deadline_t::never()) const;

  /* Use this method to close the bot instance before moving it from one local server to another. You need to
   * delete the webhook before calling this method to ensure that the bot isn't launched again after server
   * restart. The method will return error 429 in the first 10 minutes after the bot is launched. Returns True
   * on success. Requires no parameters. */
  dd::task<bool> close(deadline_t deadline = deadline_t::never()) const;

  /* Use this method to send text messages. On success, the sent Message is returned. */
  dd::task<Message> sendMessage(api::send_message_request, deadline_t deadline = deadline_t::never()) const;

  /* Use this method to forward messages of any kind. Service messages and messages with protected content
   * can't be forwarded. On success, the sent Message is returned. */
  dd::task<Message> forwardMessage(api::forward_message_request,
                                   deadline_t deadline = deadline_t::never()) const;

  /* Use this method to forward multiple messages of any kind. If some of the specified messages can't be
   * found or forwarded, they are skipped. Service messages and messages with protected content can't be
   * forwarded. Album grouping is kept for forwarded messages. On success, an array of MessageId of the sent
   * messages is returned. */
  dd::task<arrayof<MessageId>> forwardMessages(api::forward_messages_request,
                                               deadline_t deadline = deadline_t::never()) const;

  /* Use this method to copy messages of any kind. Service messages, paid media messages, giveaway messages,
   * giveaway winners messages, and invoice messages can't be copied. A quiz poll can be copied only if the
   * value of the field correct_option_id is known to the bot. The method is analogous to the method
   * forwardMessage, but the copied message doesn't have a link to the original message. Returns the MessageId
   * of the sent message on success. */
  dd::task<MessageId> copyMessage(api::copy_message_request, deadline_t deadline = deadline_t::never()) const;

  /* Use this method to copy messages of any kind. If some of the specified messages can't be found or copied,
   * they are skipped. Service messages, paid media messages, giveaway messages, giveaway winners messages,
   * and invoice messages can't be copied. A quiz poll can be copied only if the value of the field
   * correct_option_id is known to the bot. The method is analogous to the method forwardMessages, but the
   * copied messages don't have a link to the original message. Album grouping is kept for copied messages. On
   * success, an array of MessageId of the sent messages is returned. */
  dd::task<arrayof<MessageId>> copyMessages(api::copy_messages_request,
                                            deadline_t deadline = deadline_t::never()) const;

  /* Use this method to send photos. On success, the sent Message is returned. */
  dd::task<Message> sendPhoto(api::send_photo_request, deadline_t deadline = deadline_t::never()) const;

  /* Use this method to send audio files, if you want Telegram clients to display them in the music player.
   * Your audio must be in the .MP3 or .M4A format. On success, the sent Message is returned. Bots can
   * currently send audio files of up to 50 MB in size, this limit may be changed in the future. */
  dd::task<Message> sendAudio(api::send_audio_request, deadline_t deadline = deadline_t::never()) const;

  /* Use this method to send general files. On success, the sent Message is returned. Bots can currently send
   * files of any type of up to 50 MB in size, this limit may be changed in the future. */
  dd::task<Message> sendDocument(api::send_document_request, deadline_t deadline = deadline_t::never()) const;

  /* Use this method to send video files, Telegram clients support MPEG4 videos (other formats may be sent as
   * Document). On success, the sent Message is returned. Bots can currently send video files of up to 50 MB
   * in size, this limit may be changed in the future. */
  dd::task<Message> sendVideo(api::send_video_request, deadline_t deadline = deadline_t::never()) const;

  /* Use this method to send animation files (GIF or H.264/MPEG-4 AVC video without sound). On success, the
   * sent Message is returned. Bots can currently send animation files of up to 50 MB in size, this limit may
   * be changed in the future. */
  dd::task<Message> sendAnimation(api::send_animation_request,
                                  deadline_t deadline = deadline_t::never()) const;

  /* Use this method to send audio files, if you want Telegram clients to display the file as a playable voice
   * message. For this to work, your audio must be in an .OGG file encoded with OPUS, or in .MP3 format, or in
   * .M4A format (other formats may be sent as Audio or Document). On success, the sent Message is returned.
   * Bots can currently send voice messages of up to 50 MB in size, this limit may be changed in the future.
   */
  dd::task<Message> sendVoice(api::send_voice_request, deadline_t deadline = deadline_t::never()) const;

  /* As of v.4.0, Telegram clients support rounded square MPEG4 videos of up to 1 minute long. Use this method
   * to send video messages. On success, the sent Message is returned. */
  dd::task<Message> sendVideoNote(api::send_video_note_request,
                                  deadline_t deadline = deadline_t::never()) const;

  /* Use this method to send paid media. On success, the sent Message is returned. */
  dd::task<Message> sendPaidMedia(api::send_paid_media_request,
                                  deadline_t deadline = deadline_t::never()) const;

  /* Use this method to send a group of photos, videos, documents or audios as an album. Documents and audio
   * files can be only grouped in an album with messages of the same type. On success, an array of Messages
   * that were sent is returned. */
  dd::task<arrayof<Message>> sendMediaGroup(api::send_media_group_request,
                                            deadline_t deadline = deadline_t::never()) const;

  /* Use this method to send point on the map. On success, the sent Message is returned. */
  dd::task<Message> sendLocation(api::send_location_request, deadline_t deadline = deadline_t::never()) const;

  /* Use this method to send information about a venue. On success, the sent Message is returned. */
  dd::task<Message> sendVenue(api::send_venue_request, deadline_t deadline = deadline_t::never()) const;

  /* Use this method to send phone contacts. On success, the sent Message is returned. */
  dd::task<Message> sendContact(api::send_contact_request, deadline_t deadline = deadline_t::never()) const;

  /* Use this method to send a native poll. On success, the sent Message is returned. */
  dd::task<Message> sendPoll(api::send_poll_request, deadline_t deadline = deadline_t::never()) const;

  /* Use this method to send an animated emoji that will display a random value. On success, the sent Message
   * is returned. */
  dd::task<Message> sendDice(api::send_dice_request, deadline_t deadline = deadline_t::never()) const;

  /* Use this method when you need to tell the user that something is happening on the bot's side. The status
   * is set for 5 seconds or less (when a message arrives from your bot, Telegram clients clear its typing
   * status). Returns True on success. */
  dd::task<bool> sendChatAction(api::send_chat_action_request,
                                deadline_t deadline = deadline_t::never()) const;

  /* Use this method to change the chosen reactions on a message. Service messages of some types can't be
   * reacted to. Automatically forwarded messages from a channel to its discussion group have the same
   * available reactions as messages in the channel. Bots can't use paid reactions. Returns True on success.
   */
  dd::task<bool> setMessageReaction(api::set_message_reaction_request,
                                    deadline_t deadline = deadline_t::never()) const;

  /* Use this method to get a list of profile pictures for a user. Returns a UserProfilePhotos object. */
  dd::task<UserProfilePhotos> getUserProfilePhotos(api::get_user_profile_photos_request,
                                                   deadline_t deadline = deadline_t::never()) const;

  /* Changes the emoji status for a given user that previously allowed the bot to manage their emoji status
   * via the Mini App method requestEmojiStatusAccess. Returns True on success. */
  dd::task<bool> setUserEmojiStatus(api::set_user_emoji_status_request,
                                    deadline_t deadline = deadline_t::never()) const;

  /* Use this method to get basic information about a file and prepare it for downloading. For the moment,
   * bots can download files of up to 20MB in size. On success, a File object is returned. The file can then
   * be downloaded via the link https://api.telegram.org/file/bot<token>/<file_path>, where <file_path> is
   * taken from the response. It is guaranteed that the link will be valid for at least 1 hour. When the link
   * expires, a new one can be requested by calling getFile again. */
  dd::task<File> getFile(api::get_file_request, deadline_t deadline = deadline_t::never()) const;

  /* Use this method to ban a user in a group, a supergroup or a channel. In the case of supergroups and
   * channels, the user will not be able to return to the chat on their own using invite links, etc., unless
   * unbanned first. The bot must be an administrator in the chat for this to work and must have the
   * appropriate administrator rights. Returns True on success. */
  dd::task<bool> banChatMember(api::ban_chat_member_request, deadline_t deadline = deadline_t::never()) const;

  /* Use this method to unban a previously banned user in a supergroup or channel. The user will not return to
   * the group or channel automatically, but will be able to join via link, etc. The bot must be an
   * administrator for this to work. By default, this method guarantees that after the call the user is not a
   * member of the chat, but will be able to join it. So if the user is a member of the chat they will also be
   * removed from the chat. If you don't want this, use the parameter only_if_banned. Returns True on success.
   */
  dd::task<bool> unbanChatMember(api::unban_chat_member_request,
                                 deadline_t deadline = deadline_t::never()) const;

  /* Use this method to restrict a user in a supergroup. The bot must be an administrator in the supergroup
   * for this to work and must have the appropriate administrator rights. Pass True for all permissions to
   * lift restrictions from a user. Returns True on success. */
  dd::task<bool> restrictChatMember(api::restrict_chat_member_request,
                                    deadline_t deadline = deadline_t::never()) const;

  /* Use this method to promote or demote a user in a supergroup or a channel. The bot must be an
   * administrator in the chat for this to work and must have the appropriate administrator rights. Pass False
   * for all boolean parameters to demote a user. Returns True on success. */
  dd::task<bool> promoteChatMember(api::promote_chat_member_request,
                                   deadline_t deadline = deadline_t::never()) const;

  /* Use this method to set a custom title for an administrator in a supergroup promoted by the bot. Returns
   * True on success. */
  dd::task<bool> setChatAdministratorCustomTitle(api::set_chat_administrator_custom_title_request,
                                                 deadline_t deadline = deadline_t::never()) const;

  /* Use this method to ban a channel chat in a supergroup or a channel. Until the chat is unbanned, the owner
   * of the banned chat won't be able to send messages on behalf of any of their channels. The bot must be an
   * administrator in the supergroup or channel for this to work and must have the appropriate administrator
   * rights. Returns True on success. */
  dd::task<bool> banChatSenderChat(api::ban_chat_sender_chat_request,
                                   deadline_t deadline = deadline_t::never()) const;

  /* Use this method to unban a previously banned channel chat in a supergroup or channel. The bot must be an
   * administrator for this to work and must have the appropriate administrator rights. Returns True on
   * success. */
  dd::task<bool> unbanChatSenderChat(api::unban_chat_sender_chat_request,
                                     deadline_t deadline = deadline_t::never()) const;

  /* Use this method to set default chat permissions for all members. The bot must be an administrator in the
   * group or a supergroup for this to work and must have the can_restrict_members administrator rights.
   * Returns True on success. */
  dd::task<bool> setChatPermissions(api::set_chat_permissions_request,
                                    deadline_t deadline = deadline_t::never()) const;

  /* Use this method to generate a new primary invite link for a chat; any previously generated primary link
   * is revoked. The bot must be an administrator in the chat for this to work and must have the appropriate
   * administrator rights. Returns the new invite link as String on success. */
  dd::task<String> exportChatInviteLink(api::export_chat_invite_link_request,
                                        deadline_t deadline = deadline_t::never()) const;

  /* Use this method to create an additional invite link for a chat. The bot must be an administrator in the
   * chat for this to work and must have the appropriate administrator rights. The link can be revoked using
   * the method revokeChatInviteLink. Returns the new invite link as ChatInviteLink object. */
  dd::task<ChatInviteLink> createChatInviteLink(api::create_chat_invite_link_request,
                                                deadline_t deadline = deadline_t::never()) const;

  /* Use this method to edit a non-primary invite link created by the bot. The bot must be an administrator in
   * the chat for this to work and must have the appropriate administrator rights. Returns the edited invite
   * link as a ChatInviteLink object. */
  dd::task<ChatInviteLink> editChatInviteLink(api::edit_chat_invite_link_request,
                                              deadline_t deadline = deadline_t::never()) const;

  /* Use this method to create a subscription invite link for a channel chat. The bot must have the
   * can_invite_users administrator rights. The link can be edited using the method
   * editChatSubscriptionInviteLink or revoked using the method revokeChatInviteLink. Returns the new invite
   * link as a ChatInviteLink object. */
  dd::task<ChatInviteLink> createChatSubscriptionInviteLink(api::create_chat_subscription_invite_link_request,
                                                            deadline_t deadline = deadline_t::never()) const;

  /* Use this method to edit a subscription invite link created by the bot. The bot must have the
   * can_invite_users administrator rights. Returns the edited invite link as a ChatInviteLink object. */
  dd::task<ChatInviteLink> editChatSubscriptionInviteLink(api::edit_chat_subscription_invite_link_request,
                                                          deadline_t deadline = deadline_t::never()) const;

  /* Use this method to revoke an invite link created by the bot. If the primary link is revoked, a new link
   * is automatically generated. The bot must be an administrator in the chat for this to work and must have
   * the appropriate administrator rights. Returns the revoked invite link as ChatInviteLink object. */
  dd::task<ChatInviteLink> revokeChatInviteLink(api::revoke_chat_invite_link_request,
                                                deadline_t deadline = deadline_t::never()) const;

  /* Use this method to approve a chat join request. The bot must be an administrator in the chat for this to
   * work and must have the can_invite_users administrator right. Returns True on success. */
  dd::task<bool> approveChatJoinRequest(api::approve_chat_join_request_request,
                                        deadline_t deadline = deadline_t::never()) const;

  /* Use this method to decline a chat join request. The bot must be an administrator in the chat for this to
   * work and must have the can_invite_users administrator right. Returns True on success. */
  dd::task<bool> declineChatJoinRequest(api::decline_chat_join_request_request,
                                        deadline_t deadline = deadline_t::never()) const;

  /* Use this method to set a new profile photo for the chat. Photos can't be changed for private chats. The
   * bot must be an administrator in the chat for this to work and must have the appropriate administrator
   * rights. Returns True on success. */
  dd::task<bool> setChatPhoto(api::set_chat_photo_request, deadline_t deadline = deadline_t::never()) const;

  /* Use this method to delete a chat photo. Photos can't be changed for private chats. The bot must be an
   * administrator in the chat for this to work and must have the appropriate administrator rights. Returns
   * True on success. */
  dd::task<bool> deleteChatPhoto(api::delete_chat_photo_request,
                                 deadline_t deadline = deadline_t::never()) const;

  /* Use this method to change the title of a chat. Titles can't be changed for private chats. The bot must be
   * an administrator in the chat for this to work and must have the appropriate administrator rights. Returns
   * True on success. */
  dd::task<bool> setChatTitle(api::set_chat_title_request, deadline_t deadline = deadline_t::never()) const;

  /* Use this method to change the description of a group, a supergroup or a channel. The bot must be an
   * administrator in the chat for this to work and must have the appropriate administrator rights. Returns
   * True on success. */
  dd::task<bool> setChatDescription(api::set_chat_description_request,
                                    deadline_t deadline = deadline_t::never()) const;

  /* Use this method to add a message to the list of pinned messages in a chat. If the chat is not a private
   * chat, the bot must be an administrator in the chat for this to work and must have the 'can_pin_messages'
   * administrator right in a supergroup or 'can_edit_messages' administrator right in a channel. Returns True
   * on success. */
  dd::task<bool> pinChatMessage(api::pin_chat_message_request,
                                deadline_t deadline = deadline_t::never()) const;

  /* Use this method to remove a message from the list of pinned messages in a chat. If the chat is not a
   * private chat, the bot must be an administrator in the chat for this to work and must have the
   * 'can_pin_messages' administrator right in a supergroup or 'can_edit_messages' administrator right in a
   * channel. Returns True on success. */
  dd::task<bool> unpinChatMessage(api::unpin_chat_message_request,
                                  deadline_t deadline = deadline_t::never()) const;

  /* Use this method to clear the list of pinned messages in a chat. If the chat is not a private chat, the
   * bot must be an administrator in the chat for this to work and must have the 'can_pin_messages'
   * administrator right in a supergroup or 'can_edit_messages' administrator right in a channel. Returns True
   * on success. */
  dd::task<bool> unpinAllChatMessages(api::unpin_all_chat_messages_request,
                                      deadline_t deadline = deadline_t::never()) const;

  /* Use this method for your bot to leave a group, supergroup or channel. Returns True on success. */
  dd::task<bool> leaveChat(api::leave_chat_request, deadline_t deadline = deadline_t::never()) const;

  /* Use this method to get up-to-date information about the chat. Returns a ChatFullInfo object on success.
   */
  dd::task<ChatFullInfo> getChat(api::get_chat_request, deadline_t deadline = deadline_t::never()) const;

  /* Use this method to get a list of administrators in a chat, which aren't bots. Returns an Array of
   * ChatMember objects. */
  dd::task<arrayof<ChatMember>> getChatAdministrators(api::get_chat_administrators_request,
                                                      deadline_t deadline = deadline_t::never()) const;

  /* Use this method to get the number of members in a chat. Returns Int on success. */
  dd::task<Integer> getChatMemberCount(api::get_chat_member_count_request,
                                       deadline_t deadline = deadline_t::never()) const;

  /* Use this method to get information about a member of a chat. The method is only guaranteed to work for
   * other users if the bot is an administrator in the chat. Returns a ChatMember object on success. */
  dd::task<ChatMember> getChatMember(api::get_chat_member_request,
                                     deadline_t deadline = deadline_t::never()) const;

  /* Use this method to set a new group sticker set for a supergroup. The bot must be an administrator in the
   * chat for this to work and must have the appropriate administrator rights. Use the field
   * can_set_sticker_set optionally returned in getChat requests to check if the bot can use this method.
   * Returns True on success. */
  dd::task<bool> setChatStickerSet(api::set_chat_sticker_set_request,
                                   deadline_t deadline = deadline_t::never()) const;

  /* Use this method to delete a group sticker set from a supergroup. The bot must be an administrator in the
   * chat for this to work and must have the appropriate administrator rights. Use the field
   * can_set_sticker_set optionally returned in getChat requests to check if the bot can use this method.
   * Returns True on success. */
  dd::task<bool> deleteChatStickerSet(api::delete_chat_sticker_set_request,
                                      deadline_t deadline = deadline_t::never()) const;

  /* Use this method to get custom emoji stickers, which can be used as a forum topic icon by any user.
   * Requires no parameters. Returns an Array of Sticker objects. */
  dd::task<arrayof<Sticker>> getForumTopicIconStickers(deadline_t deadline = deadline_t::never()) const;

  /* Use this method to create a topic in a forum supergroup chat. The bot must be an administrator in the
   * chat for this to work and must have the can_manage_topics administrator rights. Returns information about
   * the created topic as a ForumTopic object. */
  dd::task<ForumTopic> createForumTopic(api::create_forum_topic_request,
                                        deadline_t deadline = deadline_t::never()) const;

  /* Use this method to edit name and icon of a topic in a forum supergroup chat. The bot must be an
   * administrator in the chat for this to work and must have the can_manage_topics administrator rights,
   * unless it is the creator of the topic. Returns True on success. */
  dd::task<bool> editForumTopic(api::edit_forum_topic_request,
                                deadline_t deadline = deadline_t::never()) const;

  /* Use this method to close an open topic in a forum supergroup chat. The bot must be an administrator in
   * the chat for this to work and must have the can_manage_topics administrator rights, unless it is the
   * creator of the topic. Returns True on success. */
  dd::task<bool> closeForumTopic(api::close_forum_topic_request,
                                 deadline_t deadline = deadline_t::never()) const;

  /* Use this method to reopen a closed topic in a forum supergroup chat. The bot must be an administrator in
   * the chat for this to work and must have the can_manage_topics administrator rights, unless it is the
   * creator of the topic. Returns True on success. */
  dd::task<bool> reopenForumTopic(api::reopen_forum_topic_request,
                                  deadline_t deadline = deadline_t::never()) const;

  /* Use this method to delete a forum topic along with all its messages in a forum supergroup chat. The bot
   * must be an administrator in the chat for this to work and must have the can_delete_messages administrator
   * rights. Returns True on success. */
  dd::task<bool> deleteForumTopic(api::delete_forum_topic_request,
                                  deadline_t deadline = deadline_t::never()) const;

  /* Use this method to clear the list of pinned messages in a forum topic. The bot must be an administrator
   * in the chat for this to work and must have the can_pin_messages administrator right in the supergroup.
   * Returns True on success. */
  dd::task<bool> unpinAllForumTopicMessages(api::unpin_all_forum_topic_messages_request,
                                            deadline_t deadline = deadline_t::never()) const;

  /* Use this method to edit the name of the 'General' topic in a forum supergroup chat. The bot must be an
   * administrator in the chat for this to work and must have the can_manage_topics administrator rights.
   * Returns True on success. */
  dd::task<bool> editGeneralForumTopic(api::edit_general_forum_topic_request,
                                       deadline_t deadline = deadline_t::never()) const;

  /* Use this method to close an open 'General' topic in a forum supergroup chat. The bot must be an
   * administrator in the chat for this to work and must have the can_manage_topics administrator rights.
   * Returns True on success. */
  dd::task<bool> closeGeneralForumTopic(api::close_general_forum_topic_request,
                                        deadline_t deadline = deadline_t::never()) const;

  /* Use this method to reopen a closed 'General' topic in a forum supergroup chat. The bot must be an
   * administrator in the chat for this to work and must have the can_manage_topics administrator rights. The
   * topic will be automatically unhidden if it was hidden. Returns True on success. */
  dd::task<bool> reopenGeneralForumTopic(api::reopen_general_forum_topic_request,
                                         deadline_t deadline = deadline_t::never()) const;

  /* Use this method to hide the 'General' topic in a forum supergroup chat. The bot must be an administrator
   * in the chat for this to work and must have the can_manage_topics administrator rights. The topic will be
   * automatically closed if it was open. Returns True on success. */
  dd::task<bool> hideGeneralForumTopic(api::hide_general_forum_topic_request,
                                       deadline_t deadline = deadline_t::never()) const;

  /* Use this method to unhide the 'General' topic in a forum supergroup chat. The bot must be an
   * administrator in the chat for this to work and must have the can_manage_topics administrator rights.
   * Returns True on success. */
  dd::task<bool> unhideGeneralForumTopic(api::unhide_general_forum_topic_request,
                                         deadline_t deadline = deadline_t::never()) const;

  /* Use this method to clear the list of pinned messages in a General forum topic. The bot must be an
   * administrator in the chat for this to work and must have the can_pin_messages administrator right in the
   * supergroup. Returns True on success. */
  dd::task<bool> unpinAllGeneralForumTopicMessages(api::unpin_all_general_forum_topic_messages_request,
                                                   deadline_t deadline = deadline_t::never()) const;

  /* Use this method to send answers to callback queries sent from inline keyboards. The answer will be
   * displayed to the user as a notification at the top of the chat screen or as an alert. On success, True is
   * returned. */
  dd::task<bool> answerCallbackQuery(api::answer_callback_query_request,
                                     deadline_t deadline = deadline_t::never()) const;

  /* Use this method to get the list of boosts added to a chat by a user. Requires administrator rights in the
   * chat. Returns a UserChatBoosts object. */
  dd::task<UserChatBoosts> getUserChatBoosts(api::get_user_chat_boosts_request,
                                             deadline_t deadline = deadline_t::never()) const;

  /* Use this method to get information about the connection of the bot with a business account. Returns a
   * BusinessConnection object on success. */
  dd::task<BusinessConnection> getBusinessConnection(api::get_business_connection_request,
                                                     deadline_t deadline = deadline_t::never()) const;

  /* Use this method to change the list of the bot's commands. See this manual for more details about bot
   * commands. Returns True on success. */
  dd::task<bool> setMyCommands(api::set_my_commands_request, deadline_t deadline = deadline_t::never()) const;

  /* Use this method to delete the list of the bot's commands for the given scope and user language. After
   * deletion, higher level commands will be shown to affected users. Returns True on success. */
  dd::task<bool> deleteMyCommands(api::delete_my_commands_request,
                                  deadline_t deadline = deadline_t::never()) const;

  /* Use this method to get the current list of the bot's commands for the given scope and user language.
   * Returns an Array of BotCommand objects. If commands aren't set, an empty list is returned. */
  dd::task<arrayof<BotCommand>> getMyCommands(api::get_my_commands_request,
                                              deadline_t deadline = deadline_t::never()) const;

  /* Use this method to change the bot's name. Returns True on success. */
  dd::task<bool> setMyName(api::set_my_name_request, deadline_t deadline = deadline_t::never()) const;

  /* Use this method to get the current bot name for the given user language. Returns BotName on success. */
  dd::task<BotName> getMyName(api::get_my_name_request, deadline_t deadline = deadline_t::never()) const;

  /* Use this method to change the bot's description, which is shown in the chat with the bot if the chat is
   * empty. Returns True on success. */
  dd::task<bool> setMyDescription(api::set_my_description_request,
                                  deadline_t deadline = deadline_t::never()) const;

  /* Use this method to get the current bot description for the given user language. Returns BotDescription on
   * success. */
  dd::task<BotDescription> getMyDescription(api::get_my_description_request,
                                            deadline_t deadline = deadline_t::never()) const;

  /* Use this method to change the bot's short description, which is shown on the bot's profile page and is
   * sent together with the link when users share the bot. Returns True on success. */
  dd::task<bool> setMyShortDescription(api::set_my_short_description_request,
                                       deadline_t deadline = deadline_t::never()) const;

  /* Use this method to get the current bot short description for the given user language. Returns
   * BotShortDescription on success. */
  dd::task<BotShortDescription> getMyShortDescription(api::get_my_short_description_request,
                                                      deadline_t deadline = deadline_t::never()) const;

  /* Use this method to change the bot's menu button in a private chat, or the default menu button. Returns
   * True on success. */
  dd::task<bool> setChatMenuButton(api::set_chat_menu_button_request,
                                   deadline_t deadline = deadline_t::never()) const;

  /* Use this method to get the current value of the bot's menu button in a private chat, or the default menu
   * button. Returns MenuButton on success. */
  dd::task<MenuButton> getChatMenuButton(api::get_chat_menu_button_request,
                                         deadline_t deadline = deadline_t::never()) const;

  /* Use this method to change the default administrator rights requested by the bot when it's added as an
   * administrator to groups or channels. These rights will be suggested to users, but they are free to modify
   * the list before adding the bot. Returns True on success. */
  dd::task<bool> setMyDefaultAdministratorRights(api::set_my_default_administrator_rights_request,
                                                 deadline_t deadline = deadline_t::never()) const;

  /* Use this method to get the current default administrator rights of the bot. Returns
   * ChatAdministratorRights on success. */
  dd::task<ChatAdministratorRights> getMyDefaultAdministratorRights(
      api::get_my_default_administrator_rights_request, deadline_t deadline = deadline_t::never()) const;

  /* Use this method to edit text and game messages. On success, if the edited message is not an inline
   * message, the edited Message is returned, otherwise True is returned. Note that business messages that
   * were not sent by the bot and do not contain an inline keyboard can only be edited within 48 hours from
   * the time they were sent. */
  dd::task<oneof<bool, Message>> editMessageText(api::edit_message_text_request,
                                                 deadline_t deadline = deadline_t::never()) const;

  /* Use this method to edit captions of messages. On success, if the edited message is not an inline message,
   * the edited Message is returned, otherwise True is returned. Note that business messages that were not
   * sent by the bot and do not contain an inline keyboard can only be edited within 48 hours from the time
   * they were sent. */
  dd::task<oneof<bool, Message>> editMessageCaption(api::edit_message_caption_request,
                                                    deadline_t deadline = deadline_t::never()) const;

  /* Use this method to edit animation, audio, document, photo, or video messages, or to add media to text
   * messages. If a message is part of a message album, then it can be edited only to an audio for audio
   * albums, only to a document for document albums and to a photo or a video otherwise. When an inline
   * message is edited, a new file can't be uploaded; use a previously uploaded file via its file_id or
   * specify a URL. On success, if the edited message is not an inline message, the edited Message is
   * returned, otherwise True is returned. Note that business messages that were not sent by the bot and do
   * not contain an inline keyboard can only be edited within 48 hours from the time they were sent. */
  dd::task<oneof<bool, Message>> editMessageMedia(api::edit_message_media_request,
                                                  deadline_t deadline = deadline_t::never()) const;

  /* Use this method to edit live location messages. A location can be edited until its live_period expires or
   * editing is explicitly disabled by a call to stopMessageLiveLocation. On success, if the edited message is
   * not an inline message, the edited Message is returned, otherwise True is returned. */
  dd::task<oneof<bool, Message>> editMessageLiveLocation(api::edit_message_live_location_request,
                                                         deadline_t deadline = deadline_t::never()) const;

  /* Use this method to stop updating a live location message before live_period expires. On success, if the
   * message is not an inline message, the edited Message is returned, otherwise True is returned. */
  dd::task<oneof<bool, Message>> stopMessageLiveLocation(api::stop_message_live_location_request,
                                                         deadline_t deadline = deadline_t::never()) const;

  /* Use this method to edit only the reply markup of messages. On success, if the edited message is not an
   * inline message, the edited Message is returned, otherwise True is returned. Note that business messages
   * that were not sent by the bot and do not contain an inline keyboard can only be edited within 48 hours
   * from the time they were sent. */
  dd::task<oneof<bool, Message>> editMessageReplyMarkup(api::edit_message_reply_markup_request,
                                                        deadline_t deadline = deadline_t::never()) const;

  /* Use this method to stop a poll which was sent by the bot. On success, the stopped Poll is returned. */
  dd::task<Poll> stopPoll(api::stop_poll_request, deadline_t deadline = deadline_t::never()) const;

  /* Use this method to delete a message, including service messages, with the following limitations:- A
   * message can only be deleted if it was sent less than 48 hours ago.- Service messages about a supergroup,
   * channel, or forum topic creation can't be deleted.- A dice message in a private chat can only be deleted
   * if it was sent more than 24 hours ago.- Bots can delete outgoing messages in private chats, groups, and
   * supergroups.- Bots can delete incoming messages in private chats.- Bots granted can_post_messages
   * permissions can delete outgoing messages in channels.- If the bot is an administrator of a group, it can
   * delete any message there.- If the bot has can_delete_messages permission in a supergroup or a channel, it
   * can delete any message there.Returns True on success. */
  dd::task<bool> deleteMessage(api::delete_message_request, deadline_t deadline = deadline_t::never()) const;

  /* Use this method to delete multiple messages simultaneously. If some of the specified messages can't be
   * found, they are skipped. Returns True on success. */
  dd::task<bool> deleteMessages(api::delete_messages_request,
                                deadline_t deadline = deadline_t::never()) const;

  /* Returns the list of gifts that can be sent by the bot to users and channel chats. Requires no parameters.
   * Returns a Gifts object. */
  dd::task<Gifts> getAvailableGifts(deadline_t deadline = deadline_t::never()) const;

  /* Sends a gift to the given user or channel chat. The gift can't be converted to Telegram Stars by the
   * receiver. Returns True on success. */
  dd::task<bool> sendGift(api::send_gift_request, deadline_t deadline = deadline_t::never()) const;

  /* Gifts a Telegram Premium subscription to the given user. Returns True on success. */
  dd::task<bool> giftPremiumSubscription(api::gift_premium_subscription_request,
                                         deadline_t deadline = deadline_t::never()) const;

  /* Verifies a user on behalf of the organization which is represented by the bot. Returns True on success.
   */
  dd::task<bool> verifyUser(api::verify_user_request, deadline_t deadline = deadline_t::never()) const;

  /* Verifies a chat on behalf of the organization which is represented by the bot. Returns True on success.
   */
  dd::task<bool> verifyChat(api::verify_chat_request, deadline_t deadline = deadline_t::never()) const;

  /* Removes verification from a user who is currently verified on behalf of the organization represented by
   * the bot. Returns True on success. */
  dd::task<bool> removeUserVerification(api::remove_user_verification_request,
                                        deadline_t deadline = deadline_t::never()) const;

  /* Removes verification from a chat that is currently verified on behalf of the organization represented by
   * the bot. Returns True on success. */
  dd::task<bool> removeChatVerification(api::remove_chat_verification_request,
                                        deadline_t deadline = deadline_t::never()) const;

  /* Marks incoming message as read on behalf of a business account. Requires the can_read_messages business
   * bot right. Returns True on success. */
  dd::task<bool> readBusinessMessage(api::read_business_message_request,
                                     deadline_t deadline = deadline_t::never()) const;

  /* Delete messages on behalf of a business account. Requires the can_delete_sent_messages business bot right
   * to delete messages sent by the bot itself, or the can_delete_all_messages business bot right to delete
   * any message. Returns True on success. */
  dd::task<bool> deleteBusinessMessages(api::delete_business_messages_request,
                                        deadline_t deadline = deadline_t::never()) const;

  /* Changes the first and last name of a managed business account. Requires the can_change_name business bot
   * right. Returns True on success. */
  dd::task<bool> setBusinessAccountName(api::set_business_account_name_request,
                                        deadline_t deadline = deadline_t::never()) const;

  /* Changes the username of a managed business account. Requires the can_change_username business bot right.
   * Returns True on success. */
  dd::task<bool> setBusinessAccountUsername(api::set_business_account_username_request,
                                            deadline_t deadline = deadline_t::never()) const;

  /* Changes the bio of a managed business account. Requires the can_change_bio business bot right. Returns
   * True on success. */
  dd::task<bool> setBusinessAccountBio(api::set_business_account_bio_request,
                                       deadline_t deadline = deadline_t::never()) const;

  /* Changes the profile photo of a managed business account. Requires the can_edit_profile_photo business bot
   * right. Returns True on success. */
  dd::task<bool> setBusinessAccountProfilePhoto(api::set_business_account_profile_photo_request,
                                                deadline_t deadline = deadline_t::never()) const;

  /* Removes the current profile photo of a managed business account. Requires the can_edit_profile_photo
   * business bot right. Returns True on success. */
  dd::task<bool> removeBusinessAccountProfilePhoto(api::remove_business_account_profile_photo_request,
                                                   deadline_t deadline = deadline_t::never()) const;

  /* Changes the privacy settings pertaining to incoming gifts in a managed business account. Requires the
   * can_change_gift_settings business bot right. Returns True on success. */
  dd::task<bool> setBusinessAccountGiftSettings(api::set_business_account_gift_settings_request,
                                                deadline_t deadline = deadline_t::never()) const;

  /* Returns the amount of Telegram Stars owned by a managed business account. Requires the
   * can_view_gifts_and_stars business bot right. Returns StarAmount on success. */
  dd::task<StarAmount> getBusinessAccountStarBalance(api::get_business_account_star_balance_request,
                                                     deadline_t deadline = deadline_t::never()) const;

  /* Transfers Telegram Stars from the business account balance to the bot's balance. Requires the
   * can_transfer_stars business bot right. Returns True on success. */
  dd::task<bool> transferBusinessAccountStars(api::transfer_business_account_stars_request,
                                              deadline_t deadline = deadline_t::never()) const;

  /* Returns the gifts received and owned by a managed business account. Requires the can_view_gifts_and_stars
   * business bot right. Returns OwnedGifts on success. */
  dd::task<OwnedGifts> getBusinessAccountGifts(api::get_business_account_gifts_request,
                                               deadline_t deadline = deadline_t::never()) const;

  /* Converts a given regular gift to Telegram Stars. Requires the can_convert_gifts_to_stars business bot
   * right. Returns True on success. */
  dd::task<bool> convertGiftToStars(api::convert_gift_to_stars_request,
                                    deadline_t deadline = deadline_t::never()) const;

  /* Upgrades a given regular gift to a unique gift. Requires the can_transfer_and_upgrade_gifts business bot
   * right. Additionally requires the can_transfer_stars business bot right if the upgrade is paid. Returns
   * True on success. */
  dd::task<bool> upgradeGift(api::upgrade_gift_request, deadline_t deadline = deadline_t::never()) const;

  /* Transfers an owned unique gift to another user. Requires the can_transfer_and_upgrade_gifts business bot
   * right. Requires can_transfer_stars business bot right if the transfer is paid. Returns True on success.
   */
  dd::task<bool> transferGift(api::transfer_gift_request, deadline_t deadline = deadline_t::never()) const;

  /* Posts a story on behalf of a managed business account. Requires the can_manage_stories business bot
   * right. Returns Story on success. */
  dd::task<Story> postStory(api::post_story_request, deadline_t deadline = deadline_t::never()) const;

  /* Edits a story previously posted by the bot on behalf of a managed business account. Requires the
   * can_manage_stories business bot right. Returns Story on success. */
  dd::task<Story> editStory(api::edit_story_request, deadline_t deadline = deadline_t::never()) const;

  /* Deletes a story previously posted by the bot on behalf of a managed business account. Requires the
   * can_manage_stories business bot right. Returns True on success. */
  dd::task<bool> deleteStory(api::delete_story_request, deadline_t deadline = deadline_t::never()) const;

  /* Use this method to send static .WEBP, animated .TGS, or video .WEBM stickers. On success, the sent
   * Message is returned. */
  dd::task<Message> sendSticker(api::send_sticker_request, deadline_t deadline = deadline_t::never()) const;

  /* Use this method to get a sticker set. On success, a StickerSet object is returned. */
  dd::task<StickerSet> getStickerSet(api::get_sticker_set_request,
                                     deadline_t deadline = deadline_t::never()) const;

  /* Use this method to get information about custom emoji stickers by their identifiers. Returns an Array of
   * Sticker objects. */
  dd::task<arrayof<Sticker>> getCustomEmojiStickers(api::get_custom_emoji_stickers_request,
                                                    deadline_t deadline = deadline_t::never()) const;

  /* Use this method to upload a file with a sticker for later use in the createNewStickerSet,
   * addStickerToSet, or replaceStickerInSet methods (the file can be used multiple times). Returns the
   * uploaded File on success. */
  dd::task<File> uploadStickerFile(api::upload_sticker_file_request,
                                   deadline_t deadline = deadline_t::never()) const;

  /* Use this method to create a new sticker set owned by a user. The bot will be able to edit the sticker set
   * thus created. Returns True on success. */
  dd::task<bool> createNewStickerSet(api::create_new_sticker_set_request,
                                     deadline_t deadline = deadline_t::never()) const;

  /* Use this method to add a new sticker to a set created by the bot. Emoji sticker sets can have up to 200
   * stickers. Other sticker sets can have up to 120 stickers. Returns True on success. */
  dd::task<bool> addStickerToSet(api::add_sticker_to_set_request,
                                 deadline_t deadline = deadline_t::never()) const;

  /* Use this method to move a sticker in a set created by the bot to a specific position. Returns True on
   * success. */
  dd::task<bool> setStickerPositionInSet(api::set_sticker_position_in_set_request,
                                         deadline_t deadline = deadline_t::never()) const;

  /* Use this method to delete a sticker from a set created by the bot. Returns True on success. */
  dd::task<bool> deleteStickerFromSet(api::delete_sticker_from_set_request,
                                      deadline_t deadline = deadline_t::never()) const;

  /* Use this method to replace an existing sticker in a sticker set with a new one. The method is equivalent
   * to calling deleteStickerFromSet, then addStickerToSet, then setStickerPositionInSet. Returns True on
   * success. */
  dd::task<bool> replaceStickerInSet(api::replace_sticker_in_set_request,
                                     deadline_t deadline = deadline_t::never()) const;

  /* Use this method to change the list of emoji assigned to a regular or custom emoji sticker. The sticker
   * must belong to a sticker set created by the bot. Returns True on success. */
  dd::task<bool> setStickerEmojiList(api::set_sticker_emoji_list_request,
                                     deadline_t deadline = deadline_t::never()) const;

  /* Use this method to change search keywords assigned to a regular or custom emoji sticker. The sticker must
   * belong to a sticker set created by the bot. Returns True on success. */
  dd::task<bool> setStickerKeywords(api::set_sticker_keywords_request,
                                    deadline_t deadline = deadline_t::never()) const;

  /* Use this method to change the mask position of a mask sticker. The sticker must belong to a sticker set
   * that was created by the bot. Returns True on success. */
  dd::task<bool> setStickerMaskPosition(api::set_sticker_mask_position_request,
                                        deadline_t deadline = deadline_t::never()) const;

  /* Use this method to set the title of a created sticker set. Returns True on success. */
  dd::task<bool> setStickerSetTitle(api::set_sticker_set_title_request,
                                    deadline_t deadline = deadline_t::never()) const;

  /* Use this method to set the thumbnail of a regular or mask sticker set. The format of the thumbnail file
   * must match the format of the stickers in the set. Returns True on success. */
  dd::task<bool> setStickerSetThumbnail(api::set_sticker_set_thumbnail_request,
                                        deadline_t deadline = deadline_t::never()) const;

  /* Use this method to set the thumbnail of a custom emoji sticker set. Returns True on success. */
  dd::task<bool> setCustomEmojiStickerSetThumbnail(api::set_custom_emoji_sticker_set_thumbnail_request,
                                                   deadline_t deadline = deadline_t::never()) const;

  /* Use this method to delete a sticker set that was created by the bot. Returns True on success. */
  dd::task<bool> deleteStickerSet(api::delete_sticker_set_request,
                                  deadline_t deadline = deadline_t::never()) const;

  /* Use this method to send answers to an inline query. On success, True is returned.No more than 50 results
   * per query are allowed. */
  dd::task<bool> answerInlineQuery(api::answer_inline_query_request,
                                   deadline_t deadline = deadline_t::never()) const;

  /* Use this method to set the result of an interaction with a Web App and send a corresponding message on
   * behalf of the user to the chat from which the query originated. On success, a SentWebAppMessage object is
   * returned. */
  dd::task<SentWebAppMessage> answerWebAppQuery(api::answer_web_app_query_request,
                                                deadline_t deadline = deadline_t::never()) const;

  /* Stores a message that can be sent by a user of a Mini App. Returns a PreparedInlineMessage object. */
  dd::task<PreparedInlineMessage> savePreparedInlineMessage(api::save_prepared_inline_message_request,
                                                            deadline_t deadline = deadline_t::never()) const;

  /* Use this method to send invoices. On success, the sent Message is returned. */
  dd::task<Message> sendInvoice(api::send_invoice_request, deadline_t deadline = deadline_t::never()) const;

  /* Use this method to create a link for an invoice. Returns the created invoice link as String on success.
   */
  dd::task<String> createInvoiceLink(api::create_invoice_link_request,
                                     deadline_t deadline = deadline_t::never()) const;

  /* If you sent an invoice requesting a shipping address and the parameter is_flexible was specified, the Bot
   * API will send an Update with a shipping_query field to the bot. Use this method to reply to shipping
   * queries. On success, True is returned. */
  dd::task<bool> answerShippingQuery(api::answer_shipping_query_request,
                                     deadline_t deadline = deadline_t::never()) const;

  /* Once the user has confirmed their payment and shipping details, the Bot API sends the final confirmation
   * in the form of an Update with the field pre_checkout_query. Use this method to respond to such
   * pre-checkout queries. On success, True is returned. Note: The Bot API must receive an answer within 10
   * seconds after the pre-checkout query was sent. */
  dd::task<bool> answerPreCheckoutQuery(api::answer_pre_checkout_query_request,
                                        deadline_t deadline = deadline_t::never()) const;

  /* Returns the bot's Telegram Star transactions in chronological order. On success, returns a
   * StarTransactions object. */
  dd::task<StarTransactions> getStarTransactions(api::get_star_transactions_request,
                                                 deadline_t deadline = deadline_t::never()) const;

  /* Refunds a successful payment in Telegram Stars. Returns True on success. */
  dd::task<bool> refundStarPayment(api::refund_star_payment_request,
                                   deadline_t deadline = deadline_t::never()) const;

  /* Allows the bot to cancel or re-enable extension of a subscription paid in Telegram Stars. Returns True on
   * success. */
  dd::task<bool> editUserStarSubscription(api::edit_user_star_subscription_request,
                                          deadline_t deadline = deadline_t::never()) const;

  /* Informs a user that some of the Telegram Passport elements they provided contains errors. The user will
   * not be able to re-submit their Passport to you until the errors are fixed (the contents of the field for
   * which you returned the error must change). Returns True on success. */
  dd::task<bool> setPassportDataErrors(api::set_passport_data_errors_request,
                                       deadline_t deadline = deadline_t::never()) const;

  /* Use this method to send a game. On success, the sent Message is returned. */
  dd::task<Message> sendGame(api::send_game_request, deadline_t deadline = deadline_t::never()) const;

  /* Use this method to set the score of the specified user in a game message. On success, if the message is
   * not an inline message, the Message is returned, otherwise True is returned. Returns an error, if the new
   * score is not greater than the user's current score in the chat and force is False. */
  dd::task<oneof<bool, Message>> setGameScore(api::set_game_score_request,
                                              deadline_t deadline = deadline_t::never()) const;

  /* Use this method to get data for high score tables. Will return the score of the specified user and
   * several of their neighbors in a game. Returns an Array of GameHighScore objects. */
  dd::task<arrayof<GameHighScore>> getGameHighScores(api::get_game_high_scores_request,
                                                     deadline_t deadline = deadline_t::never()) const;
};

}  // namespace tgbm::api
