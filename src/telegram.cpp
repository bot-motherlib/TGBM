#include <tgbm/api/telegram.hpp>

#include <tgbm/api/types/all.hpp>
#include <tgbm/api/methods/all.hpp>
#include <tgbm/api/const_string.hpp>
#include <tgbm/api/requests.hpp>

#include <tgbm/utils/deadline.hpp>

namespace tgbm::api {

dd::task<arrayof<Update>> telegram::getUpdates(api::get_updates_request request, deadline_t deadline) const {
  arrayof<Update>& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("getUpdates request ended with error, status: {}, description: {}", err.status,
                   err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<bool> telegram::setWebhook(api::set_webhook_request request, deadline_t deadline) const {
  bool& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("setWebhook request ended with error, status: {}, description: {}", err.status,
                   err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<bool> telegram::deleteWebhook(api::delete_webhook_request request, deadline_t deadline) const {
  bool& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("deleteWebhook request ended with error, status: {}, description: {}", err.status,
                   err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<WebhookInfo> telegram::getWebhookInfo(deadline_t deadline) const {
  WebhookInfo& result = co_await dd::this_coro::return_place;
  reqerr_t err =
      co_await api::send_request(api::get_webhook_info_request{}, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("getWebhookInfo request ended with error, status: {}, description: {}", err.status,
                   err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<User> telegram::getMe(deadline_t deadline) const {
  User& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(api::get_me_request{}, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("getMe request ended with error, status: {}, description: {}", err.status,
                   err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<bool> telegram::logOut(deadline_t deadline) const {
  bool& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(api::log_out_request{}, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("logOut request ended with error, status: {}, description: {}", err.status,
                   err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<bool> telegram::close(deadline_t deadline) const {
  bool& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(api::close_request{}, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("close request ended with error, status: {}, description: {}", err.status,
                   err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<Message> telegram::sendMessage(api::send_message_request request, deadline_t deadline) const {
  Message& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("sendMessage request ended with error, status: {}, description: {}", err.status,
                   err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<Message> telegram::forwardMessage(api::forward_message_request request, deadline_t deadline) const {
  Message& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("forwardMessage request ended with error, status: {}, description: {}", err.status,
                   err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<arrayof<MessageId>> telegram::forwardMessages(api::forward_messages_request request,
                                                       deadline_t deadline) const {
  arrayof<MessageId>& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("forwardMessages request ended with error, status: {}, description: {}", err.status,
                   err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<MessageId> telegram::copyMessage(api::copy_message_request request, deadline_t deadline) const {
  MessageId& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("copyMessage request ended with error, status: {}, description: {}", err.status,
                   err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<arrayof<MessageId>> telegram::copyMessages(api::copy_messages_request request,
                                                    deadline_t deadline) const {
  arrayof<MessageId>& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("copyMessages request ended with error, status: {}, description: {}", err.status,
                   err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<Message> telegram::sendPhoto(api::send_photo_request request, deadline_t deadline) const {
  Message& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("sendPhoto request ended with error, status: {}, description: {}", err.status,
                   err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<Message> telegram::sendAudio(api::send_audio_request request, deadline_t deadline) const {
  Message& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("sendAudio request ended with error, status: {}, description: {}", err.status,
                   err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<Message> telegram::sendDocument(api::send_document_request request, deadline_t deadline) const {
  Message& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("sendDocument request ended with error, status: {}, description: {}", err.status,
                   err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<Message> telegram::sendVideo(api::send_video_request request, deadline_t deadline) const {
  Message& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("sendVideo request ended with error, status: {}, description: {}", err.status,
                   err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<Message> telegram::sendAnimation(api::send_animation_request request, deadline_t deadline) const {
  Message& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("sendAnimation request ended with error, status: {}, description: {}", err.status,
                   err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<Message> telegram::sendVoice(api::send_voice_request request, deadline_t deadline) const {
  Message& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("sendVoice request ended with error, status: {}, description: {}", err.status,
                   err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<Message> telegram::sendVideoNote(api::send_video_note_request request, deadline_t deadline) const {
  Message& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("sendVideoNote request ended with error, status: {}, description: {}", err.status,
                   err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<Message> telegram::sendPaidMedia(api::send_paid_media_request request, deadline_t deadline) const {
  Message& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("sendPaidMedia request ended with error, status: {}, description: {}", err.status,
                   err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<arrayof<Message>> telegram::sendMediaGroup(api::send_media_group_request request,
                                                    deadline_t deadline) const {
  arrayof<Message>& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("sendMediaGroup request ended with error, status: {}, description: {}", err.status,
                   err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<Message> telegram::sendLocation(api::send_location_request request, deadline_t deadline) const {
  Message& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("sendLocation request ended with error, status: {}, description: {}", err.status,
                   err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<Message> telegram::sendVenue(api::send_venue_request request, deadline_t deadline) const {
  Message& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("sendVenue request ended with error, status: {}, description: {}", err.status,
                   err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<Message> telegram::sendContact(api::send_contact_request request, deadline_t deadline) const {
  Message& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("sendContact request ended with error, status: {}, description: {}", err.status,
                   err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<Message> telegram::sendPoll(api::send_poll_request request, deadline_t deadline) const {
  Message& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("sendPoll request ended with error, status: {}, description: {}", err.status,
                   err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<Message> telegram::sendDice(api::send_dice_request request, deadline_t deadline) const {
  Message& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("sendDice request ended with error, status: {}, description: {}", err.status,
                   err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<bool> telegram::sendChatAction(api::send_chat_action_request request, deadline_t deadline) const {
  bool& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("sendChatAction request ended with error, status: {}, description: {}", err.status,
                   err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<bool> telegram::setMessageReaction(api::set_message_reaction_request request,
                                            deadline_t deadline) const {
  bool& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("setMessageReaction request ended with error, status: {}, description: {}", err.status,
                   err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<UserProfilePhotos> telegram::getUserProfilePhotos(api::get_user_profile_photos_request request,
                                                           deadline_t deadline) const {
  UserProfilePhotos& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("getUserProfilePhotos request ended with error, status: {}, description: {}", err.status,
                   err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<bool> telegram::setUserEmojiStatus(api::set_user_emoji_status_request request,
                                            deadline_t deadline) const {
  bool& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("setUserEmojiStatus request ended with error, status: {}, description: {}", err.status,
                   err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<File> telegram::getFile(api::get_file_request request, deadline_t deadline) const {
  File& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("getFile request ended with error, status: {}, description: {}", err.status,
                   err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<bool> telegram::banChatMember(api::ban_chat_member_request request, deadline_t deadline) const {
  bool& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("banChatMember request ended with error, status: {}, description: {}", err.status,
                   err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<bool> telegram::unbanChatMember(api::unban_chat_member_request request, deadline_t deadline) const {
  bool& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("unbanChatMember request ended with error, status: {}, description: {}", err.status,
                   err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<bool> telegram::restrictChatMember(api::restrict_chat_member_request request,
                                            deadline_t deadline) const {
  bool& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("restrictChatMember request ended with error, status: {}, description: {}", err.status,
                   err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<bool> telegram::promoteChatMember(api::promote_chat_member_request request,
                                           deadline_t deadline) const {
  bool& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("promoteChatMember request ended with error, status: {}, description: {}", err.status,
                   err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<bool> telegram::setChatAdministratorCustomTitle(
    api::set_chat_administrator_custom_title_request request, deadline_t deadline) const {
  bool& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("setChatAdministratorCustomTitle request ended with error, status: {}, description: {}",
                   err.status, err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<bool> telegram::banChatSenderChat(api::ban_chat_sender_chat_request request,
                                           deadline_t deadline) const {
  bool& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("banChatSenderChat request ended with error, status: {}, description: {}", err.status,
                   err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<bool> telegram::unbanChatSenderChat(api::unban_chat_sender_chat_request request,
                                             deadline_t deadline) const {
  bool& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("unbanChatSenderChat request ended with error, status: {}, description: {}", err.status,
                   err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<bool> telegram::setChatPermissions(api::set_chat_permissions_request request,
                                            deadline_t deadline) const {
  bool& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("setChatPermissions request ended with error, status: {}, description: {}", err.status,
                   err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<String> telegram::exportChatInviteLink(api::export_chat_invite_link_request request,
                                                deadline_t deadline) const {
  String& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("exportChatInviteLink request ended with error, status: {}, description: {}", err.status,
                   err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<ChatInviteLink> telegram::createChatInviteLink(api::create_chat_invite_link_request request,
                                                        deadline_t deadline) const {
  ChatInviteLink& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("createChatInviteLink request ended with error, status: {}, description: {}", err.status,
                   err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<ChatInviteLink> telegram::editChatInviteLink(api::edit_chat_invite_link_request request,
                                                      deadline_t deadline) const {
  ChatInviteLink& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("editChatInviteLink request ended with error, status: {}, description: {}", err.status,
                   err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<ChatInviteLink> telegram::createChatSubscriptionInviteLink(
    api::create_chat_subscription_invite_link_request request, deadline_t deadline) const {
  ChatInviteLink& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("createChatSubscriptionInviteLink request ended with error, status: {}, description: {}",
                   err.status, err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<ChatInviteLink> telegram::editChatSubscriptionInviteLink(
    api::edit_chat_subscription_invite_link_request request, deadline_t deadline) const {
  ChatInviteLink& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("editChatSubscriptionInviteLink request ended with error, status: {}, description: {}",
                   err.status, err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<ChatInviteLink> telegram::revokeChatInviteLink(api::revoke_chat_invite_link_request request,
                                                        deadline_t deadline) const {
  ChatInviteLink& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("revokeChatInviteLink request ended with error, status: {}, description: {}", err.status,
                   err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<bool> telegram::approveChatJoinRequest(api::approve_chat_join_request_request request,
                                                deadline_t deadline) const {
  bool& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("approveChatJoinRequest request ended with error, status: {}, description: {}", err.status,
                   err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<bool> telegram::declineChatJoinRequest(api::decline_chat_join_request_request request,
                                                deadline_t deadline) const {
  bool& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("declineChatJoinRequest request ended with error, status: {}, description: {}", err.status,
                   err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<bool> telegram::setChatPhoto(api::set_chat_photo_request request, deadline_t deadline) const {
  bool& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("setChatPhoto request ended with error, status: {}, description: {}", err.status,
                   err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<bool> telegram::deleteChatPhoto(api::delete_chat_photo_request request, deadline_t deadline) const {
  bool& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("deleteChatPhoto request ended with error, status: {}, description: {}", err.status,
                   err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<bool> telegram::setChatTitle(api::set_chat_title_request request, deadline_t deadline) const {
  bool& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("setChatTitle request ended with error, status: {}, description: {}", err.status,
                   err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<bool> telegram::setChatDescription(api::set_chat_description_request request,
                                            deadline_t deadline) const {
  bool& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("setChatDescription request ended with error, status: {}, description: {}", err.status,
                   err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<bool> telegram::pinChatMessage(api::pin_chat_message_request request, deadline_t deadline) const {
  bool& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("pinChatMessage request ended with error, status: {}, description: {}", err.status,
                   err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<bool> telegram::unpinChatMessage(api::unpin_chat_message_request request,
                                          deadline_t deadline) const {
  bool& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("unpinChatMessage request ended with error, status: {}, description: {}", err.status,
                   err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<bool> telegram::unpinAllChatMessages(api::unpin_all_chat_messages_request request,
                                              deadline_t deadline) const {
  bool& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("unpinAllChatMessages request ended with error, status: {}, description: {}", err.status,
                   err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<bool> telegram::leaveChat(api::leave_chat_request request, deadline_t deadline) const {
  bool& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("leaveChat request ended with error, status: {}, description: {}", err.status,
                   err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<ChatFullInfo> telegram::getChat(api::get_chat_request request, deadline_t deadline) const {
  ChatFullInfo& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("getChat request ended with error, status: {}, description: {}", err.status,
                   err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<arrayof<ChatMember>> telegram::getChatAdministrators(api::get_chat_administrators_request request,
                                                              deadline_t deadline) const {
  arrayof<ChatMember>& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("getChatAdministrators request ended with error, status: {}, description: {}", err.status,
                   err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<Integer> telegram::getChatMemberCount(api::get_chat_member_count_request request,
                                               deadline_t deadline) const {
  Integer& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("getChatMemberCount request ended with error, status: {}, description: {}", err.status,
                   err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<ChatMember> telegram::getChatMember(api::get_chat_member_request request,
                                             deadline_t deadline) const {
  ChatMember& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("getChatMember request ended with error, status: {}, description: {}", err.status,
                   err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<bool> telegram::setChatStickerSet(api::set_chat_sticker_set_request request,
                                           deadline_t deadline) const {
  bool& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("setChatStickerSet request ended with error, status: {}, description: {}", err.status,
                   err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<bool> telegram::deleteChatStickerSet(api::delete_chat_sticker_set_request request,
                                              deadline_t deadline) const {
  bool& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("deleteChatStickerSet request ended with error, status: {}, description: {}", err.status,
                   err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<arrayof<Sticker>> telegram::getForumTopicIconStickers(deadline_t deadline) const {
  arrayof<Sticker>& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(api::get_forum_topic_icon_stickers_request{}, client, bottoken,
                                            result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("getForumTopicIconStickers request ended with error, status: {}, description: {}",
                   err.status, err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<ForumTopic> telegram::createForumTopic(api::create_forum_topic_request request,
                                                deadline_t deadline) const {
  ForumTopic& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("createForumTopic request ended with error, status: {}, description: {}", err.status,
                   err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<bool> telegram::editForumTopic(api::edit_forum_topic_request request, deadline_t deadline) const {
  bool& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("editForumTopic request ended with error, status: {}, description: {}", err.status,
                   err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<bool> telegram::closeForumTopic(api::close_forum_topic_request request, deadline_t deadline) const {
  bool& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("closeForumTopic request ended with error, status: {}, description: {}", err.status,
                   err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<bool> telegram::reopenForumTopic(api::reopen_forum_topic_request request,
                                          deadline_t deadline) const {
  bool& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("reopenForumTopic request ended with error, status: {}, description: {}", err.status,
                   err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<bool> telegram::deleteForumTopic(api::delete_forum_topic_request request,
                                          deadline_t deadline) const {
  bool& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("deleteForumTopic request ended with error, status: {}, description: {}", err.status,
                   err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<bool> telegram::unpinAllForumTopicMessages(api::unpin_all_forum_topic_messages_request request,
                                                    deadline_t deadline) const {
  bool& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("unpinAllForumTopicMessages request ended with error, status: {}, description: {}",
                   err.status, err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<bool> telegram::editGeneralForumTopic(api::edit_general_forum_topic_request request,
                                               deadline_t deadline) const {
  bool& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("editGeneralForumTopic request ended with error, status: {}, description: {}", err.status,
                   err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<bool> telegram::closeGeneralForumTopic(api::close_general_forum_topic_request request,
                                                deadline_t deadline) const {
  bool& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("closeGeneralForumTopic request ended with error, status: {}, description: {}", err.status,
                   err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<bool> telegram::reopenGeneralForumTopic(api::reopen_general_forum_topic_request request,
                                                 deadline_t deadline) const {
  bool& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("reopenGeneralForumTopic request ended with error, status: {}, description: {}",
                   err.status, err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<bool> telegram::hideGeneralForumTopic(api::hide_general_forum_topic_request request,
                                               deadline_t deadline) const {
  bool& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("hideGeneralForumTopic request ended with error, status: {}, description: {}", err.status,
                   err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<bool> telegram::unhideGeneralForumTopic(api::unhide_general_forum_topic_request request,
                                                 deadline_t deadline) const {
  bool& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("unhideGeneralForumTopic request ended with error, status: {}, description: {}",
                   err.status, err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<bool> telegram::unpinAllGeneralForumTopicMessages(
    api::unpin_all_general_forum_topic_messages_request request, deadline_t deadline) const {
  bool& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("unpinAllGeneralForumTopicMessages request ended with error, status: {}, description: {}",
                   err.status, err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<bool> telegram::answerCallbackQuery(api::answer_callback_query_request request,
                                             deadline_t deadline) const {
  bool& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("answerCallbackQuery request ended with error, status: {}, description: {}", err.status,
                   err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<UserChatBoosts> telegram::getUserChatBoosts(api::get_user_chat_boosts_request request,
                                                     deadline_t deadline) const {
  UserChatBoosts& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("getUserChatBoosts request ended with error, status: {}, description: {}", err.status,
                   err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<BusinessConnection> telegram::getBusinessConnection(api::get_business_connection_request request,
                                                             deadline_t deadline) const {
  BusinessConnection& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("getBusinessConnection request ended with error, status: {}, description: {}", err.status,
                   err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<bool> telegram::setMyCommands(api::set_my_commands_request request, deadline_t deadline) const {
  bool& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("setMyCommands request ended with error, status: {}, description: {}", err.status,
                   err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<bool> telegram::deleteMyCommands(api::delete_my_commands_request request,
                                          deadline_t deadline) const {
  bool& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("deleteMyCommands request ended with error, status: {}, description: {}", err.status,
                   err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<arrayof<BotCommand>> telegram::getMyCommands(api::get_my_commands_request request,
                                                      deadline_t deadline) const {
  arrayof<BotCommand>& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("getMyCommands request ended with error, status: {}, description: {}", err.status,
                   err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<bool> telegram::setMyName(api::set_my_name_request request, deadline_t deadline) const {
  bool& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("setMyName request ended with error, status: {}, description: {}", err.status,
                   err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<BotName> telegram::getMyName(api::get_my_name_request request, deadline_t deadline) const {
  BotName& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("getMyName request ended with error, status: {}, description: {}", err.status,
                   err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<bool> telegram::setMyDescription(api::set_my_description_request request,
                                          deadline_t deadline) const {
  bool& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("setMyDescription request ended with error, status: {}, description: {}", err.status,
                   err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<BotDescription> telegram::getMyDescription(api::get_my_description_request request,
                                                    deadline_t deadline) const {
  BotDescription& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("getMyDescription request ended with error, status: {}, description: {}", err.status,
                   err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<bool> telegram::setMyShortDescription(api::set_my_short_description_request request,
                                               deadline_t deadline) const {
  bool& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("setMyShortDescription request ended with error, status: {}, description: {}", err.status,
                   err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<BotShortDescription> telegram::getMyShortDescription(api::get_my_short_description_request request,
                                                              deadline_t deadline) const {
  BotShortDescription& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("getMyShortDescription request ended with error, status: {}, description: {}", err.status,
                   err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<bool> telegram::setChatMenuButton(api::set_chat_menu_button_request request,
                                           deadline_t deadline) const {
  bool& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("setChatMenuButton request ended with error, status: {}, description: {}", err.status,
                   err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<MenuButton> telegram::getChatMenuButton(api::get_chat_menu_button_request request,
                                                 deadline_t deadline) const {
  MenuButton& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("getChatMenuButton request ended with error, status: {}, description: {}", err.status,
                   err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<bool> telegram::setMyDefaultAdministratorRights(
    api::set_my_default_administrator_rights_request request, deadline_t deadline) const {
  bool& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("setMyDefaultAdministratorRights request ended with error, status: {}, description: {}",
                   err.status, err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<ChatAdministratorRights> telegram::getMyDefaultAdministratorRights(
    api::get_my_default_administrator_rights_request request, deadline_t deadline) const {
  ChatAdministratorRights& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("getMyDefaultAdministratorRights request ended with error, status: {}, description: {}",
                   err.status, err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<oneof<bool, Message>> telegram::editMessageText(api::edit_message_text_request request,
                                                         deadline_t deadline) const {
  oneof<bool, Message>& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("editMessageText request ended with error, status: {}, description: {}", err.status,
                   err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<oneof<bool, Message>> telegram::editMessageCaption(api::edit_message_caption_request request,
                                                            deadline_t deadline) const {
  oneof<bool, Message>& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("editMessageCaption request ended with error, status: {}, description: {}", err.status,
                   err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<oneof<bool, Message>> telegram::editMessageMedia(api::edit_message_media_request request,
                                                          deadline_t deadline) const {
  oneof<bool, Message>& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("editMessageMedia request ended with error, status: {}, description: {}", err.status,
                   err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<oneof<bool, Message>> telegram::editMessageLiveLocation(
    api::edit_message_live_location_request request, deadline_t deadline) const {
  oneof<bool, Message>& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("editMessageLiveLocation request ended with error, status: {}, description: {}",
                   err.status, err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<oneof<bool, Message>> telegram::stopMessageLiveLocation(
    api::stop_message_live_location_request request, deadline_t deadline) const {
  oneof<bool, Message>& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("stopMessageLiveLocation request ended with error, status: {}, description: {}",
                   err.status, err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<oneof<bool, Message>> telegram::editMessageReplyMarkup(
    api::edit_message_reply_markup_request request, deadline_t deadline) const {
  oneof<bool, Message>& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("editMessageReplyMarkup request ended with error, status: {}, description: {}", err.status,
                   err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<Poll> telegram::stopPoll(api::stop_poll_request request, deadline_t deadline) const {
  Poll& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("stopPoll request ended with error, status: {}, description: {}", err.status,
                   err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<bool> telegram::deleteMessage(api::delete_message_request request, deadline_t deadline) const {
  bool& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("deleteMessage request ended with error, status: {}, description: {}", err.status,
                   err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<bool> telegram::deleteMessages(api::delete_messages_request request, deadline_t deadline) const {
  bool& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("deleteMessages request ended with error, status: {}, description: {}", err.status,
                   err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<Message> telegram::sendSticker(api::send_sticker_request request, deadline_t deadline) const {
  Message& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("sendSticker request ended with error, status: {}, description: {}", err.status,
                   err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<StickerSet> telegram::getStickerSet(api::get_sticker_set_request request,
                                             deadline_t deadline) const {
  StickerSet& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("getStickerSet request ended with error, status: {}, description: {}", err.status,
                   err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<arrayof<Sticker>> telegram::getCustomEmojiStickers(api::get_custom_emoji_stickers_request request,
                                                            deadline_t deadline) const {
  arrayof<Sticker>& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("getCustomEmojiStickers request ended with error, status: {}, description: {}", err.status,
                   err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<File> telegram::uploadStickerFile(api::upload_sticker_file_request request,
                                           deadline_t deadline) const {
  File& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("uploadStickerFile request ended with error, status: {}, description: {}", err.status,
                   err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<bool> telegram::createNewStickerSet(api::create_new_sticker_set_request request,
                                             deadline_t deadline) const {
  bool& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("createNewStickerSet request ended with error, status: {}, description: {}", err.status,
                   err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<bool> telegram::addStickerToSet(api::add_sticker_to_set_request request, deadline_t deadline) const {
  bool& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("addStickerToSet request ended with error, status: {}, description: {}", err.status,
                   err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<bool> telegram::setStickerPositionInSet(api::set_sticker_position_in_set_request request,
                                                 deadline_t deadline) const {
  bool& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("setStickerPositionInSet request ended with error, status: {}, description: {}",
                   err.status, err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<bool> telegram::deleteStickerFromSet(api::delete_sticker_from_set_request request,
                                              deadline_t deadline) const {
  bool& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("deleteStickerFromSet request ended with error, status: {}, description: {}", err.status,
                   err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<bool> telegram::replaceStickerInSet(api::replace_sticker_in_set_request request,
                                             deadline_t deadline) const {
  bool& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("replaceStickerInSet request ended with error, status: {}, description: {}", err.status,
                   err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<bool> telegram::setStickerEmojiList(api::set_sticker_emoji_list_request request,
                                             deadline_t deadline) const {
  bool& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("setStickerEmojiList request ended with error, status: {}, description: {}", err.status,
                   err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<bool> telegram::setStickerKeywords(api::set_sticker_keywords_request request,
                                            deadline_t deadline) const {
  bool& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("setStickerKeywords request ended with error, status: {}, description: {}", err.status,
                   err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<bool> telegram::setStickerMaskPosition(api::set_sticker_mask_position_request request,
                                                deadline_t deadline) const {
  bool& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("setStickerMaskPosition request ended with error, status: {}, description: {}", err.status,
                   err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<bool> telegram::setStickerSetTitle(api::set_sticker_set_title_request request,
                                            deadline_t deadline) const {
  bool& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("setStickerSetTitle request ended with error, status: {}, description: {}", err.status,
                   err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<bool> telegram::setStickerSetThumbnail(api::set_sticker_set_thumbnail_request request,
                                                deadline_t deadline) const {
  bool& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("setStickerSetThumbnail request ended with error, status: {}, description: {}", err.status,
                   err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<bool> telegram::setCustomEmojiStickerSetThumbnail(
    api::set_custom_emoji_sticker_set_thumbnail_request request, deadline_t deadline) const {
  bool& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("setCustomEmojiStickerSetThumbnail request ended with error, status: {}, description: {}",
                   err.status, err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<bool> telegram::deleteStickerSet(api::delete_sticker_set_request request,
                                          deadline_t deadline) const {
  bool& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("deleteStickerSet request ended with error, status: {}, description: {}", err.status,
                   err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<Gifts> telegram::getAvailableGifts(deadline_t deadline) const {
  Gifts& result = co_await dd::this_coro::return_place;
  reqerr_t err =
      co_await api::send_request(api::get_available_gifts_request{}, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("getAvailableGifts request ended with error, status: {}, description: {}", err.status,
                   err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<bool> telegram::sendGift(api::send_gift_request request, deadline_t deadline) const {
  bool& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("sendGift request ended with error, status: {}, description: {}", err.status,
                   err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<bool> telegram::verifyUser(api::verify_user_request request, deadline_t deadline) const {
  bool& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("verifyUser request ended with error, status: {}, description: {}", err.status,
                   err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<bool> telegram::verifyChat(api::verify_chat_request request, deadline_t deadline) const {
  bool& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("verifyChat request ended with error, status: {}, description: {}", err.status,
                   err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<bool> telegram::removeUserVerification(api::remove_user_verification_request request,
                                                deadline_t deadline) const {
  bool& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("removeUserVerification request ended with error, status: {}, description: {}", err.status,
                   err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<bool> telegram::removeChatVerification(api::remove_chat_verification_request request,
                                                deadline_t deadline) const {
  bool& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("removeChatVerification request ended with error, status: {}, description: {}", err.status,
                   err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<bool> telegram::answerInlineQuery(api::answer_inline_query_request request,
                                           deadline_t deadline) const {
  bool& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("answerInlineQuery request ended with error, status: {}, description: {}", err.status,
                   err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<SentWebAppMessage> telegram::answerWebAppQuery(api::answer_web_app_query_request request,
                                                        deadline_t deadline) const {
  SentWebAppMessage& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("answerWebAppQuery request ended with error, status: {}, description: {}", err.status,
                   err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<PreparedInlineMessage> telegram::savePreparedInlineMessage(
    api::save_prepared_inline_message_request request, deadline_t deadline) const {
  PreparedInlineMessage& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("savePreparedInlineMessage request ended with error, status: {}, description: {}",
                   err.status, err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<Message> telegram::sendInvoice(api::send_invoice_request request, deadline_t deadline) const {
  Message& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("sendInvoice request ended with error, status: {}, description: {}", err.status,
                   err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<String> telegram::createInvoiceLink(api::create_invoice_link_request request,
                                             deadline_t deadline) const {
  String& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("createInvoiceLink request ended with error, status: {}, description: {}", err.status,
                   err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<bool> telegram::answerShippingQuery(api::answer_shipping_query_request request,
                                             deadline_t deadline) const {
  bool& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("answerShippingQuery request ended with error, status: {}, description: {}", err.status,
                   err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<bool> telegram::answerPreCheckoutQuery(api::answer_pre_checkout_query_request request,
                                                deadline_t deadline) const {
  bool& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("answerPreCheckoutQuery request ended with error, status: {}, description: {}", err.status,
                   err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<StarTransactions> telegram::getStarTransactions(api::get_star_transactions_request request,
                                                         deadline_t deadline) const {
  StarTransactions& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("getStarTransactions request ended with error, status: {}, description: {}", err.status,
                   err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<bool> telegram::refundStarPayment(api::refund_star_payment_request request,
                                           deadline_t deadline) const {
  bool& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("refundStarPayment request ended with error, status: {}, description: {}", err.status,
                   err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<bool> telegram::editUserStarSubscription(api::edit_user_star_subscription_request request,
                                                  deadline_t deadline) const {
  bool& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("editUserStarSubscription request ended with error, status: {}, description: {}",
                   err.status, err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<bool> telegram::setPassportDataErrors(api::set_passport_data_errors_request request,
                                               deadline_t deadline) const {
  bool& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("setPassportDataErrors request ended with error, status: {}, description: {}", err.status,
                   err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<Message> telegram::sendGame(api::send_game_request request, deadline_t deadline) const {
  Message& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("sendGame request ended with error, status: {}, description: {}", err.status,
                   err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<oneof<bool, Message>> telegram::setGameScore(api::set_game_score_request request,
                                                      deadline_t deadline) const {
  oneof<bool, Message>& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("setGameScore request ended with error, status: {}, description: {}", err.status,
                   err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

dd::task<arrayof<GameHighScore>> telegram::getGameHighScores(api::get_game_high_scores_request request,
                                                             deadline_t deadline) const {
  arrayof<GameHighScore>& result = co_await dd::this_coro::return_place;
  reqerr_t err = co_await api::send_request(request, client, bottoken, result, deadline);
  if (err) [[unlikely]] {
    TGBM_LOG_ERROR("getGameHighScores request ended with error, status: {}, description: {}", err.status,
                   err.description.str());
    handle_telegram_http_status(err.status);
  }
  co_return dd::rvo;
}

}  // namespace tgbm::api
