#pragma once

#include "tgbm/types/Update.h"
#include "tgbm/types/WebhookInfo.h"
#include "tgbm/types/User.h"
#include "tgbm/types/Chat.h"
#include "tgbm/types/Message.h"
#include "tgbm/types/MessageId.h"
#include "tgbm/types/InaccessibleMessage.h"
#include "tgbm/types/MessageEntity.h"
#include "tgbm/types/TextQuote.h"
#include "tgbm/types/ExternalReplyInfo.h"
#include "tgbm/types/ReplyParameters.h"
#include "tgbm/types/MessageOrigin.h"
#include "tgbm/types/MessageOriginUser.h"
#include "tgbm/types/MessageOriginHiddenUser.h"
#include "tgbm/types/MessageOriginChat.h"
#include "tgbm/types/MessageOriginChannel.h"
#include "tgbm/types/PhotoSize.h"
#include "tgbm/types/Animation.h"
#include "tgbm/types/Audio.h"
#include "tgbm/types/Document.h"
#include "tgbm/types/Story.h"
#include "tgbm/types/Video.h"
#include "tgbm/types/VideoNote.h"
#include "tgbm/types/Voice.h"
#include "tgbm/types/Contact.h"
#include "tgbm/types/Dice.h"
#include "tgbm/types/PollOption.h"
#include "tgbm/types/PollAnswer.h"
#include "tgbm/types/Poll.h"
#include "tgbm/types/Location.h"
#include "tgbm/types/Venue.h"
#include "tgbm/types/WebAppData.h"
#include "tgbm/types/ProximityAlertTriggered.h"
#include "tgbm/types/MessageAutoDeleteTimerChanged.h"
#include "tgbm/types/ChatBoostAdded.h"
#include "tgbm/types/ForumTopicCreated.h"
#include "tgbm/types/ForumTopicClosed.h"
#include "tgbm/types/ForumTopicEdited.h"
#include "tgbm/types/ForumTopicReopened.h"
#include "tgbm/types/GeneralForumTopicHidden.h"
#include "tgbm/types/GeneralForumTopicUnhidden.h"
#include "tgbm/types/SharedUser.h"
#include "tgbm/types/UsersShared.h"
#include "tgbm/types/ChatShared.h"
#include "tgbm/types/WriteAccessAllowed.h"
#include "tgbm/types/VideoChatScheduled.h"
#include "tgbm/types/VideoChatStarted.h"
#include "tgbm/types/VideoChatEnded.h"
#include "tgbm/types/VideoChatParticipantsInvited.h"
#include "tgbm/types/GiveawayCreated.h"
#include "tgbm/types/Giveaway.h"
#include "tgbm/types/GiveawayWinners.h"
#include "tgbm/types/GiveawayCompleted.h"
#include "tgbm/types/LinkPreviewOptions.h"
#include "tgbm/types/UserProfilePhotos.h"
#include "tgbm/types/File.h"
#include "tgbm/types/WebAppInfo.h"
#include "tgbm/types/ReplyKeyboardMarkup.h"
#include "tgbm/types/KeyboardButton.h"
#include "tgbm/types/KeyboardButtonRequestUsers.h"
#include "tgbm/types/KeyboardButtonRequestChat.h"
#include "tgbm/types/KeyboardButtonPollType.h"
#include "tgbm/types/ReplyKeyboardRemove.h"
#include "tgbm/types/InlineKeyboardMarkup.h"
#include "tgbm/types/InlineKeyboardButton.h"
#include "tgbm/types/LoginUrl.h"
#include "tgbm/types/SwitchInlineQueryChosenChat.h"
#include "tgbm/types/CallbackQuery.h"
#include "tgbm/types/ForceReply.h"
#include "tgbm/types/ChatPhoto.h"
#include "tgbm/types/ChatInviteLink.h"
#include "tgbm/types/ChatAdministratorRights.h"
#include "tgbm/types/ChatMemberUpdated.h"
#include "tgbm/types/ChatMember.h"
#include "tgbm/types/ChatMemberOwner.h"
#include "tgbm/types/ChatMemberAdministrator.h"
#include "tgbm/types/ChatMemberMember.h"
#include "tgbm/types/ChatMemberRestricted.h"
#include "tgbm/types/ChatMemberLeft.h"
#include "tgbm/types/ChatMemberBanned.h"
#include "tgbm/types/ChatJoinRequest.h"
#include "tgbm/types/ChatPermissions.h"
#include "tgbm/types/Birthdate.h"
#include "tgbm/types/BusinessIntro.h"
#include "tgbm/types/BusinessLocation.h"
#include "tgbm/types/BusinessOpeningHoursInterval.h"
#include "tgbm/types/BusinessOpeningHours.h"
#include "tgbm/types/ChatLocation.h"
#include "tgbm/types/ReactionType.h"
#include "tgbm/types/ReactionTypeEmoji.h"
#include "tgbm/types/ReactionTypeCustomEmoji.h"
#include "tgbm/types/ReactionCount.h"
#include "tgbm/types/MessageReactionUpdated.h"
#include "tgbm/types/MessageReactionCountUpdated.h"
#include "tgbm/types/ForumTopic.h"
#include "tgbm/types/BotCommand.h"
#include "tgbm/types/BotCommandScope.h"
#include "tgbm/types/BotCommandScopeDefault.h"
#include "tgbm/types/BotCommandScopeAllPrivateChats.h"
#include "tgbm/types/BotCommandScopeAllGroupChats.h"
#include "tgbm/types/BotCommandScopeAllChatAdministrators.h"
#include "tgbm/types/BotCommandScopeChat.h"
#include "tgbm/types/BotCommandScopeChatAdministrators.h"
#include "tgbm/types/BotCommandScopeChatMember.h"
#include "tgbm/types/BotName.h"
#include "tgbm/types/BotDescription.h"
#include "tgbm/types/BotShortDescription.h"
#include "tgbm/types/MenuButton.h"
#include "tgbm/types/MenuButtonCommands.h"
#include "tgbm/types/MenuButtonWebApp.h"
#include "tgbm/types/MenuButtonDefault.h"
#include "tgbm/types/ChatBoostSource.h"
#include "tgbm/types/ChatBoostSourcePremium.h"
#include "tgbm/types/ChatBoostSourceGiftCode.h"
#include "tgbm/types/ChatBoostSourceGiveaway.h"
#include "tgbm/types/ChatBoost.h"
#include "tgbm/types/ChatBoostUpdated.h"
#include "tgbm/types/ChatBoostRemoved.h"
#include "tgbm/types/UserChatBoosts.h"
#include "tgbm/types/BusinessConnection.h"
#include "tgbm/types/BusinessMessagesDeleted.h"
#include "tgbm/types/ResponseParameters.h"
#include "tgbm/types/InputMedia.h"
#include "tgbm/types/InputMediaPhoto.h"
#include "tgbm/types/InputMediaVideo.h"
#include "tgbm/types/InputMediaAnimation.h"
#include "tgbm/types/InputMediaAudio.h"
#include "tgbm/types/InputMediaDocument.h"
#include "tgbm/types/Sticker.h"
#include "tgbm/types/StickerSet.h"
#include "tgbm/types/MaskPosition.h"
#include "tgbm/types/InputSticker.h"
#include "tgbm/types/InlineQuery.h"
#include "tgbm/types/InlineQueryResultsButton.h"
#include "tgbm/types/InlineQueryResult.h"
#include "tgbm/types/InlineQueryResultArticle.h"
#include "tgbm/types/InlineQueryResultPhoto.h"
#include "tgbm/types/InlineQueryResultGif.h"
#include "tgbm/types/InlineQueryResultMpeg4Gif.h"
#include "tgbm/types/InlineQueryResultVideo.h"
#include "tgbm/types/InlineQueryResultAudio.h"
#include "tgbm/types/InlineQueryResultVoice.h"
#include "tgbm/types/InlineQueryResultDocument.h"
#include "tgbm/types/InlineQueryResultLocation.h"
#include "tgbm/types/InlineQueryResultVenue.h"
#include "tgbm/types/InlineQueryResultContact.h"
#include "tgbm/types/InlineQueryResultGame.h"
#include "tgbm/types/InlineQueryResultCachedPhoto.h"
#include "tgbm/types/InlineQueryResultCachedGif.h"
#include "tgbm/types/InlineQueryResultCachedMpeg4Gif.h"
#include "tgbm/types/InlineQueryResultCachedSticker.h"
#include "tgbm/types/InlineQueryResultCachedDocument.h"
#include "tgbm/types/InlineQueryResultCachedVideo.h"
#include "tgbm/types/InlineQueryResultCachedVoice.h"
#include "tgbm/types/InlineQueryResultCachedAudio.h"
#include "tgbm/types/InputMessageContent.h"
#include "tgbm/types/InputTextMessageContent.h"
#include "tgbm/types/InputLocationMessageContent.h"
#include "tgbm/types/InputVenueMessageContent.h"
#include "tgbm/types/InputContactMessageContent.h"
#include "tgbm/types/InputInvoiceMessageContent.h"
#include "tgbm/types/ChosenInlineResult.h"
#include "tgbm/types/SentWebAppMessage.h"
#include "tgbm/types/LabeledPrice.h"
#include "tgbm/types/Invoice.h"
#include "tgbm/types/ShippingAddress.h"
#include "tgbm/types/OrderInfo.h"
#include "tgbm/types/ShippingOption.h"
#include "tgbm/types/SuccessfulPayment.h"
#include "tgbm/types/ShippingQuery.h"
#include "tgbm/types/PreCheckoutQuery.h"
#include "tgbm/types/PassportData.h"
#include "tgbm/types/PassportFile.h"
#include "tgbm/types/EncryptedPassportElement.h"
#include "tgbm/types/EncryptedCredentials.h"
#include "tgbm/types/PassportElementError.h"
#include "tgbm/types/PassportElementErrorDataField.h"
#include "tgbm/types/PassportElementErrorFrontSide.h"
#include "tgbm/types/PassportElementErrorReverseSide.h"
#include "tgbm/types/PassportElementErrorSelfie.h"
#include "tgbm/types/PassportElementErrorFile.h"
#include "tgbm/types/PassportElementErrorFiles.h"
#include "tgbm/types/PassportElementErrorTranslationFile.h"
#include "tgbm/types/PassportElementErrorTranslationFiles.h"
#include "tgbm/types/PassportElementErrorUnspecified.h"
#include "tgbm/types/Game.h"
#include "tgbm/types/CallbackGame.h"
#include "tgbm/types/GameHighScore.h"
#include "tgbm/types/GenericReply.h"

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include <memory>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace tgbm {

class TgTypeParser {
 public:
  template <typename T>
  using JsonToTgTypeFunc = std::shared_ptr<T> (TgTypeParser::*)(const boost::property_tree::ptree&) const;

  template <typename T>
  using TgTypeToJsonFunc = std::string (TgTypeParser::*)(const std::shared_ptr<T>&) const;

  Update::Ptr parseJsonAndGetUpdate(const boost::property_tree::ptree& data) const;
  std::string parseUpdate(const Update::Ptr& object) const;

  WebhookInfo::Ptr parseJsonAndGetWebhookInfo(const boost::property_tree::ptree& data) const;
  std::string parseWebhookInfo(const WebhookInfo::Ptr& object) const;

  User::Ptr parseJsonAndGetUser(const boost::property_tree::ptree& data) const;
  std::string parseUser(const User::Ptr& object) const;

  Chat::Ptr parseJsonAndGetChat(const boost::property_tree::ptree& data) const;
  std::string parseChat(const Chat::Ptr& object) const;

  Message::Ptr parseJsonAndGetMessage(const boost::property_tree::ptree& data) const;
  std::string parseMessage(const Message::Ptr& object) const;

  MessageId::Ptr parseJsonAndGetMessageId(const boost::property_tree::ptree& data) const;
  std::string parseMessageId(const MessageId::Ptr& object) const;

  InaccessibleMessage::Ptr parseJsonAndGetInaccessibleMessage(const boost::property_tree::ptree& data) const;
  std::string parseInaccessibleMessage(const InaccessibleMessage::Ptr& object) const;

  MessageEntity::Ptr parseJsonAndGetMessageEntity(const boost::property_tree::ptree& data) const;
  std::string parseMessageEntity(const MessageEntity::Ptr& object) const;

  TextQuote::Ptr parseJsonAndGetTextQuote(const boost::property_tree::ptree& data) const;
  std::string parseTextQuote(const TextQuote::Ptr& object) const;

  ExternalReplyInfo::Ptr parseJsonAndGetExternalReplyInfo(const boost::property_tree::ptree& data) const;
  std::string parseExternalReplyInfo(const ExternalReplyInfo::Ptr& object) const;

  ReplyParameters::Ptr parseJsonAndGetReplyParameters(const boost::property_tree::ptree& data) const;
  std::string parseReplyParameters(const ReplyParameters::Ptr& object) const;

  MessageOrigin::Ptr parseJsonAndGetMessageOrigin(const boost::property_tree::ptree& data) const;
  std::string parseMessageOrigin(const MessageOrigin::Ptr& object) const;

  MessageOriginUser::Ptr parseJsonAndGetMessageOriginUser(const boost::property_tree::ptree& data) const;
  std::string parseMessageOriginUser(const MessageOriginUser::Ptr& object) const;

  MessageOriginHiddenUser::Ptr parseJsonAndGetMessageOriginHiddenUser(
      const boost::property_tree::ptree& data) const;
  std::string parseMessageOriginHiddenUser(const MessageOriginHiddenUser::Ptr& object) const;

  MessageOriginChat::Ptr parseJsonAndGetMessageOriginChat(const boost::property_tree::ptree& data) const;
  std::string parseMessageOriginChat(const MessageOriginChat::Ptr& object) const;

  MessageOriginChannel::Ptr parseJsonAndGetMessageOriginChannel(
      const boost::property_tree::ptree& data) const;
  std::string parseMessageOriginChannel(const MessageOriginChannel::Ptr& object) const;

  PhotoSize::Ptr parseJsonAndGetPhotoSize(const boost::property_tree::ptree& data) const;
  std::string parsePhotoSize(const PhotoSize::Ptr& object) const;

  Animation::Ptr parseJsonAndGetAnimation(const boost::property_tree::ptree& data) const;
  std::string parseAnimation(const Animation::Ptr& object) const;

  Audio::Ptr parseJsonAndGetAudio(const boost::property_tree::ptree& data) const;
  std::string parseAudio(const Audio::Ptr& object) const;

  Document::Ptr parseJsonAndGetDocument(const boost::property_tree::ptree& data) const;
  std::string parseDocument(const Document::Ptr& object) const;

  Story::Ptr parseJsonAndGetStory(const boost::property_tree::ptree& data) const;
  std::string parseStory(const Story::Ptr& object) const;

  Video::Ptr parseJsonAndGetVideo(const boost::property_tree::ptree& data) const;
  std::string parseVideo(const Video::Ptr& object) const;

  VideoNote::Ptr parseJsonAndGetVideoNote(const boost::property_tree::ptree& data) const;
  std::string parseVideoNote(const VideoNote::Ptr& object) const;

  Voice::Ptr parseJsonAndGetVoice(const boost::property_tree::ptree& data) const;
  std::string parseVoice(const Voice::Ptr& object) const;

  Contact::Ptr parseJsonAndGetContact(const boost::property_tree::ptree& data) const;
  std::string parseContact(const Contact::Ptr& object) const;

  Dice::Ptr parseJsonAndGetDice(const boost::property_tree::ptree& data) const;
  std::string parseDice(const Dice::Ptr& object) const;

  PollOption::Ptr parseJsonAndGetPollOption(const boost::property_tree::ptree& data) const;
  std::string parsePollOption(const PollOption::Ptr& object) const;

  PollAnswer::Ptr parseJsonAndGetPollAnswer(const boost::property_tree::ptree& data) const;
  std::string parsePollAnswer(const PollAnswer::Ptr& object) const;

  Poll::Ptr parseJsonAndGetPoll(const boost::property_tree::ptree& data) const;
  std::string parsePoll(const Poll::Ptr& object) const;

  Location::Ptr parseJsonAndGetLocation(const boost::property_tree::ptree& data) const;
  std::string parseLocation(const Location::Ptr& object) const;

  Venue::Ptr parseJsonAndGetVenue(const boost::property_tree::ptree& data) const;
  std::string parseVenue(const Venue::Ptr& object) const;

  WebAppData::Ptr parseJsonAndGetWebAppData(const boost::property_tree::ptree& data) const;
  std::string parseWebAppData(const WebAppData::Ptr& object) const;

  ProximityAlertTriggered::Ptr parseJsonAndGetProximityAlertTriggered(
      const boost::property_tree::ptree& data) const;
  std::string parseProximityAlertTriggered(const ProximityAlertTriggered::Ptr& object) const;

  MessageAutoDeleteTimerChanged::Ptr parseJsonAndGetMessageAutoDeleteTimerChanged(
      const boost::property_tree::ptree& data) const;
  std::string parseMessageAutoDeleteTimerChanged(const MessageAutoDeleteTimerChanged::Ptr& object) const;

  ChatBoostAdded::Ptr parseJsonAndGetChatBoostAdded(const boost::property_tree::ptree& data) const;
  std::string parseChatBoostAdded(const ChatBoostAdded::Ptr& object) const;

  ForumTopicCreated::Ptr parseJsonAndGetForumTopicCreated(const boost::property_tree::ptree& data) const;
  std::string parseForumTopicCreated(const ForumTopicCreated::Ptr& object) const;

  ForumTopicClosed::Ptr parseJsonAndGetForumTopicClosed(const boost::property_tree::ptree& data) const;
  std::string parseForumTopicClosed(const ForumTopicClosed::Ptr& object) const;

  ForumTopicEdited::Ptr parseJsonAndGetForumTopicEdited(const boost::property_tree::ptree& data) const;
  std::string parseForumTopicEdited(const ForumTopicEdited::Ptr& object) const;

  ForumTopicReopened::Ptr parseJsonAndGetForumTopicReopened(const boost::property_tree::ptree& data) const;
  std::string parseForumTopicReopened(const ForumTopicReopened::Ptr& object) const;

  GeneralForumTopicHidden::Ptr parseJsonAndGetGeneralForumTopicHidden(
      const boost::property_tree::ptree& data) const;
  std::string parseGeneralForumTopicHidden(const GeneralForumTopicHidden::Ptr& object) const;

  GeneralForumTopicUnhidden::Ptr parseJsonAndGetGeneralForumTopicUnhidden(
      const boost::property_tree::ptree& data) const;
  std::string parseGeneralForumTopicUnhidden(const GeneralForumTopicUnhidden::Ptr& object) const;

  SharedUser::Ptr parseJsonAndGetSharedUser(const boost::property_tree::ptree& data) const;
  std::string parseSharedUser(const SharedUser::Ptr& object) const;

  UsersShared::Ptr parseJsonAndGetUsersShared(const boost::property_tree::ptree& data) const;
  std::string parseUsersShared(const UsersShared::Ptr& object) const;

  ChatShared::Ptr parseJsonAndGetChatShared(const boost::property_tree::ptree& data) const;
  std::string parseChatShared(const ChatShared::Ptr& object) const;

  WriteAccessAllowed::Ptr parseJsonAndGetWriteAccessAllowed(const boost::property_tree::ptree& data) const;
  std::string parseWriteAccessAllowed(const WriteAccessAllowed::Ptr& object) const;

  VideoChatScheduled::Ptr parseJsonAndGetVideoChatScheduled(const boost::property_tree::ptree& data) const;
  std::string parseVideoChatScheduled(const VideoChatScheduled::Ptr& object) const;

  VideoChatStarted::Ptr parseJsonAndGetVideoChatStarted(const boost::property_tree::ptree& data) const;
  std::string parseVideoChatStarted(const VideoChatStarted::Ptr& object) const;

  VideoChatEnded::Ptr parseJsonAndGetVideoChatEnded(const boost::property_tree::ptree& data) const;
  std::string parseVideoChatEnded(const VideoChatEnded::Ptr& object) const;

  VideoChatParticipantsInvited::Ptr parseJsonAndGetVideoChatParticipantsInvited(
      const boost::property_tree::ptree& data) const;
  std::string parseVideoChatParticipantsInvited(const VideoChatParticipantsInvited::Ptr& object) const;

  GiveawayCreated::Ptr parseJsonAndGetGiveawayCreated(const boost::property_tree::ptree& data) const;
  std::string parseGiveawayCreated(const GiveawayCreated::Ptr& object) const;

  Giveaway::Ptr parseJsonAndGetGiveaway(const boost::property_tree::ptree& data) const;
  std::string parseGiveaway(const Giveaway::Ptr& object) const;

  GiveawayWinners::Ptr parseJsonAndGetGiveawayWinners(const boost::property_tree::ptree& data) const;
  std::string parseGiveawayWinners(const GiveawayWinners::Ptr& object) const;

  GiveawayCompleted::Ptr parseJsonAndGetGiveawayCompleted(const boost::property_tree::ptree& data) const;
  std::string parseGiveawayCompleted(const GiveawayCompleted::Ptr& object) const;

  LinkPreviewOptions::Ptr parseJsonAndGetLinkPreviewOptions(const boost::property_tree::ptree& data) const;
  std::string parseLinkPreviewOptions(const LinkPreviewOptions::Ptr& object) const;

  UserProfilePhotos::Ptr parseJsonAndGetUserProfilePhotos(const boost::property_tree::ptree& data) const;
  std::string parseUserProfilePhotos(const UserProfilePhotos::Ptr& object) const;

  File::Ptr parseJsonAndGetFile(const boost::property_tree::ptree& data) const;
  std::string parseFile(const File::Ptr& object) const;

  WebAppInfo::Ptr parseJsonAndGetWebAppInfo(const boost::property_tree::ptree& data) const;
  std::string parseWebAppInfo(const WebAppInfo::Ptr& object) const;

  ReplyKeyboardMarkup::Ptr parseJsonAndGetReplyKeyboardMarkup(const boost::property_tree::ptree& data) const;
  std::string parseReplyKeyboardMarkup(const ReplyKeyboardMarkup::Ptr& object) const;

  KeyboardButton::Ptr parseJsonAndGetKeyboardButton(const boost::property_tree::ptree& data) const;
  std::string parseKeyboardButton(const KeyboardButton::Ptr& object) const;

  KeyboardButtonRequestUsers::Ptr parseJsonAndGetKeyboardButtonRequestUsers(
      const boost::property_tree::ptree& data) const;
  std::string parseKeyboardButtonRequestUsers(const KeyboardButtonRequestUsers::Ptr& object) const;

  KeyboardButtonRequestChat::Ptr parseJsonAndGetKeyboardButtonRequestChat(
      const boost::property_tree::ptree& data) const;
  std::string parseKeyboardButtonRequestChat(const KeyboardButtonRequestChat::Ptr& object) const;

  KeyboardButtonPollType::Ptr parseJsonAndGetKeyboardButtonPollType(
      const boost::property_tree::ptree& data) const;
  std::string parseKeyboardButtonPollType(const KeyboardButtonPollType::Ptr& object) const;

  ReplyKeyboardRemove::Ptr parseJsonAndGetReplyKeyboardRemove(const boost::property_tree::ptree& data) const;
  std::string parseReplyKeyboardRemove(const ReplyKeyboardRemove::Ptr& object) const;

  InlineKeyboardMarkup::Ptr parseJsonAndGetInlineKeyboardMarkup(
      const boost::property_tree::ptree& data) const;
  std::string parseInlineKeyboardMarkup(const InlineKeyboardMarkup::Ptr& object) const;

  InlineKeyboardButton::Ptr parseJsonAndGetInlineKeyboardButton(
      const boost::property_tree::ptree& data) const;
  std::string parseInlineKeyboardButton(const InlineKeyboardButton::Ptr& object) const;

  LoginUrl::Ptr parseJsonAndGetLoginUrl(const boost::property_tree::ptree& data) const;
  std::string parseLoginUrl(const LoginUrl::Ptr& object) const;

  SwitchInlineQueryChosenChat::Ptr parseJsonAndGetSwitchInlineQueryChosenChat(
      const boost::property_tree::ptree& data) const;
  std::string parseSwitchInlineQueryChosenChat(const SwitchInlineQueryChosenChat::Ptr& object) const;

  CallbackQuery::Ptr parseJsonAndGetCallbackQuery(const boost::property_tree::ptree& data) const;
  std::string parseCallbackQuery(const CallbackQuery::Ptr& object) const;

  ForceReply::Ptr parseJsonAndGetForceReply(const boost::property_tree::ptree& data) const;
  std::string parseForceReply(const ForceReply::Ptr& object) const;

  ChatPhoto::Ptr parseJsonAndGetChatPhoto(const boost::property_tree::ptree& data) const;
  std::string parseChatPhoto(const ChatPhoto::Ptr& object) const;

  ChatInviteLink::Ptr parseJsonAndGetChatInviteLink(const boost::property_tree::ptree& data) const;
  std::string parseChatInviteLink(const ChatInviteLink::Ptr& object) const;

  ChatAdministratorRights::Ptr parseJsonAndGetChatAdministratorRights(
      const boost::property_tree::ptree& data) const;
  std::string parseChatAdministratorRights(const ChatAdministratorRights::Ptr& object) const;

  ChatMemberUpdated::Ptr parseJsonAndGetChatMemberUpdated(const boost::property_tree::ptree& data) const;
  std::string parseChatMemberUpdated(const ChatMemberUpdated::Ptr& object) const;

  ChatMember::Ptr parseJsonAndGetChatMember(const boost::property_tree::ptree& data) const;
  std::string parseChatMember(const ChatMember::Ptr& object) const;

  ChatMemberOwner::Ptr parseJsonAndGetChatMemberOwner(const boost::property_tree::ptree& data) const;
  std::string parseChatMemberOwner(const ChatMemberOwner::Ptr& object) const;

  ChatMemberAdministrator::Ptr parseJsonAndGetChatMemberAdministrator(
      const boost::property_tree::ptree& data) const;
  std::string parseChatMemberAdministrator(const ChatMemberAdministrator::Ptr& object) const;

  ChatMemberMember::Ptr parseJsonAndGetChatMemberMember(const boost::property_tree::ptree& data) const;
  std::string parseChatMemberMember(const ChatMemberMember::Ptr& object) const;

  ChatMemberRestricted::Ptr parseJsonAndGetChatMemberRestricted(
      const boost::property_tree::ptree& data) const;
  std::string parseChatMemberRestricted(const ChatMemberRestricted::Ptr& object) const;

  ChatMemberLeft::Ptr parseJsonAndGetChatMemberLeft(const boost::property_tree::ptree& data) const;
  std::string parseChatMemberLeft(const ChatMemberLeft::Ptr& object) const;

  ChatMemberBanned::Ptr parseJsonAndGetChatMemberBanned(const boost::property_tree::ptree& data) const;
  std::string parseChatMemberBanned(const ChatMemberBanned::Ptr& object) const;

  ChatJoinRequest::Ptr parseJsonAndGetChatJoinRequest(const boost::property_tree::ptree& data) const;
  std::string parseChatJoinRequest(const ChatJoinRequest::Ptr& object) const;

  ChatPermissions::Ptr parseJsonAndGetChatPermissions(const boost::property_tree::ptree& data) const;
  std::string parseChatPermissions(const ChatPermissions::Ptr& object) const;

  Birthdate::Ptr parseJsonAndGetBirthdate(const boost::property_tree::ptree& data) const;
  std::string parseBirthdate(const Birthdate::Ptr& object) const;

  BusinessIntro::Ptr parseJsonAndGetBusinessIntro(const boost::property_tree::ptree& data) const;
  std::string parseBusinessIntro(const BusinessIntro::Ptr& object) const;

  BusinessLocation::Ptr parseJsonAndGetBusinessLocation(const boost::property_tree::ptree& data) const;
  std::string parseBusinessLocation(const BusinessLocation::Ptr& object) const;

  BusinessOpeningHoursInterval::Ptr parseJsonAndGetBusinessOpeningHoursInterval(
      const boost::property_tree::ptree& data) const;
  std::string parseBusinessOpeningHoursInterval(const BusinessOpeningHoursInterval::Ptr& object) const;

  BusinessOpeningHours::Ptr parseJsonAndGetBusinessOpeningHours(
      const boost::property_tree::ptree& data) const;
  std::string parseBusinessOpeningHours(const BusinessOpeningHours::Ptr& object) const;

  ChatLocation::Ptr parseJsonAndGetChatLocation(const boost::property_tree::ptree& data) const;
  std::string parseChatLocation(const ChatLocation::Ptr& object) const;

  ReactionType::Ptr parseJsonAndGetReactionType(const boost::property_tree::ptree& data) const;
  std::string parseReactionType(const ReactionType::Ptr& object) const;

  ReactionTypeEmoji::Ptr parseJsonAndGetReactionTypeEmoji(const boost::property_tree::ptree& data) const;
  std::string parseReactionTypeEmoji(const ReactionTypeEmoji::Ptr& object) const;

  ReactionTypeCustomEmoji::Ptr parseJsonAndGetReactionTypeCustomEmoji(
      const boost::property_tree::ptree& data) const;
  std::string parseReactionTypeCustomEmoji(const ReactionTypeCustomEmoji::Ptr& object) const;

  ReactionCount::Ptr parseJsonAndGetReactionCount(const boost::property_tree::ptree& data) const;
  std::string parseReactionCount(const ReactionCount::Ptr& object) const;

  MessageReactionUpdated::Ptr parseJsonAndGetMessageReactionUpdated(
      const boost::property_tree::ptree& data) const;
  std::string parseMessageReactionUpdated(const MessageReactionUpdated::Ptr& object) const;

  MessageReactionCountUpdated::Ptr parseJsonAndGetMessageReactionCountUpdated(
      const boost::property_tree::ptree& data) const;
  std::string parseMessageReactionCountUpdated(const MessageReactionCountUpdated::Ptr& object) const;

  ForumTopic::Ptr parseJsonAndGetForumTopic(const boost::property_tree::ptree& data) const;
  std::string parseForumTopic(const ForumTopic::Ptr& object) const;

  BotCommand::Ptr parseJsonAndGetBotCommand(const boost::property_tree::ptree& data) const;
  std::string parseBotCommand(const BotCommand::Ptr& object) const;

  BotCommandScope::Ptr parseJsonAndGetBotCommandScope(const boost::property_tree::ptree& data) const;
  std::string parseBotCommandScope(const BotCommandScope::Ptr& object) const;

  BotCommandScopeDefault::Ptr parseJsonAndGetBotCommandScopeDefault(
      const boost::property_tree::ptree& data) const;
  std::string parseBotCommandScopeDefault(const BotCommandScopeDefault::Ptr& object) const;

  BotCommandScopeAllPrivateChats::Ptr parseJsonAndGetBotCommandScopeAllPrivateChats(
      const boost::property_tree::ptree& data) const;
  std::string parseBotCommandScopeAllPrivateChats(const BotCommandScopeAllPrivateChats::Ptr& object) const;

  BotCommandScopeAllGroupChats::Ptr parseJsonAndGetBotCommandScopeAllGroupChats(
      const boost::property_tree::ptree& data) const;
  std::string parseBotCommandScopeAllGroupChats(const BotCommandScopeAllGroupChats::Ptr& object) const;

  BotCommandScopeAllChatAdministrators::Ptr parseJsonAndGetBotCommandScopeAllChatAdministrators(
      const boost::property_tree::ptree& data) const;
  std::string parseBotCommandScopeAllChatAdministrators(
      const BotCommandScopeAllChatAdministrators::Ptr& object) const;

  BotCommandScopeChat::Ptr parseJsonAndGetBotCommandScopeChat(const boost::property_tree::ptree& data) const;
  std::string parseBotCommandScopeChat(const BotCommandScopeChat::Ptr& object) const;

  BotCommandScopeChatAdministrators::Ptr parseJsonAndGetBotCommandScopeChatAdministrators(
      const boost::property_tree::ptree& data) const;
  std::string parseBotCommandScopeChatAdministrators(
      const BotCommandScopeChatAdministrators::Ptr& object) const;

  BotCommandScopeChatMember::Ptr parseJsonAndGetBotCommandScopeChatMember(
      const boost::property_tree::ptree& data) const;
  std::string parseBotCommandScopeChatMember(const BotCommandScopeChatMember::Ptr& object) const;

  BotName::Ptr parseJsonAndGetBotName(const boost::property_tree::ptree& data) const;
  std::string parseBotName(const BotName::Ptr& object) const;

  BotDescription::Ptr parseJsonAndGetBotDescription(const boost::property_tree::ptree& data) const;
  std::string parseBotDescription(const BotDescription::Ptr& object) const;

  BotShortDescription::Ptr parseJsonAndGetBotShortDescription(const boost::property_tree::ptree& data) const;
  std::string parseBotShortDescription(const BotShortDescription::Ptr& object) const;

  MenuButton::Ptr parseJsonAndGetMenuButton(const boost::property_tree::ptree& data) const;
  std::string parseMenuButton(const MenuButton::Ptr& object) const;

  MenuButtonCommands::Ptr parseJsonAndGetMenuButtonCommands(const boost::property_tree::ptree& data) const;
  std::string parseMenuButtonCommands(const MenuButtonCommands::Ptr& object) const;

  MenuButtonWebApp::Ptr parseJsonAndGetMenuButtonWebApp(const boost::property_tree::ptree& data) const;
  std::string parseMenuButtonWebApp(const MenuButtonWebApp::Ptr& object) const;

  MenuButtonDefault::Ptr parseJsonAndGetMenuButtonDefault(const boost::property_tree::ptree& data) const;
  std::string parseMenuButtonDefault(const MenuButtonDefault::Ptr& object) const;

  ChatBoostSource::Ptr parseJsonAndGetChatBoostSource(const boost::property_tree::ptree& data) const;
  std::string parseChatBoostSource(const ChatBoostSource::Ptr& object) const;

  ChatBoostSourcePremium::Ptr parseJsonAndGetChatBoostSourcePremium(
      const boost::property_tree::ptree& data) const;
  std::string parseChatBoostSourcePremium(const ChatBoostSourcePremium::Ptr& object) const;

  ChatBoostSourceGiftCode::Ptr parseJsonAndGetChatBoostSourceGiftCode(
      const boost::property_tree::ptree& data) const;
  std::string parseChatBoostSourceGiftCode(const ChatBoostSourceGiftCode::Ptr& object) const;

  ChatBoostSourceGiveaway::Ptr parseJsonAndGetChatBoostSourceGiveaway(
      const boost::property_tree::ptree& data) const;
  std::string parseChatBoostSourceGiveaway(const ChatBoostSourceGiveaway::Ptr& object) const;

  ChatBoost::Ptr parseJsonAndGetChatBoost(const boost::property_tree::ptree& data) const;
  std::string parseChatBoost(const ChatBoost::Ptr& object) const;

  ChatBoostUpdated::Ptr parseJsonAndGetChatBoostUpdated(const boost::property_tree::ptree& data) const;
  std::string parseChatBoostUpdated(const ChatBoostUpdated::Ptr& object) const;

  ChatBoostRemoved::Ptr parseJsonAndGetChatBoostRemoved(const boost::property_tree::ptree& data) const;
  std::string parseChatBoostRemoved(const ChatBoostRemoved::Ptr& object) const;

  UserChatBoosts::Ptr parseJsonAndGetUserChatBoosts(const boost::property_tree::ptree& data) const;
  std::string parseUserChatBoosts(const UserChatBoosts::Ptr& object) const;

  BusinessConnection::Ptr parseJsonAndGetBusinessConnection(const boost::property_tree::ptree& data) const;
  std::string parseBusinessConnection(const BusinessConnection::Ptr& object) const;

  BusinessMessagesDeleted::Ptr parseJsonAndGetBusinessMessagesDeleted(
      const boost::property_tree::ptree& data) const;
  std::string parseBusinessMessagesDeleted(const BusinessMessagesDeleted::Ptr& object) const;

  ResponseParameters::Ptr parseJsonAndGetResponseParameters(const boost::property_tree::ptree& data) const;
  std::string parseResponseParameters(const ResponseParameters::Ptr& object) const;

  InputMedia::Ptr parseJsonAndGetInputMedia(const boost::property_tree::ptree& data) const;
  std::string parseInputMedia(const InputMedia::Ptr& object) const;

  InputMediaPhoto::Ptr parseJsonAndGetInputMediaPhoto(const boost::property_tree::ptree& data) const;
  std::string parseInputMediaPhoto(const InputMediaPhoto::Ptr& object) const;

  InputMediaVideo::Ptr parseJsonAndGetInputMediaVideo(const boost::property_tree::ptree& data) const;
  std::string parseInputMediaVideo(const InputMediaVideo::Ptr& object) const;

  InputMediaAnimation::Ptr parseJsonAndGetInputMediaAnimation(const boost::property_tree::ptree& data) const;
  std::string parseInputMediaAnimation(const InputMediaAnimation::Ptr& object) const;

  InputMediaAudio::Ptr parseJsonAndGetInputMediaAudio(const boost::property_tree::ptree& data) const;
  std::string parseInputMediaAudio(const InputMediaAudio::Ptr& object) const;

  InputMediaDocument::Ptr parseJsonAndGetInputMediaDocument(const boost::property_tree::ptree& data) const;
  std::string parseInputMediaDocument(const InputMediaDocument::Ptr& object) const;

  Sticker::Ptr parseJsonAndGetSticker(const boost::property_tree::ptree& data) const;
  std::string parseSticker(const Sticker::Ptr& object) const;

  StickerSet::Ptr parseJsonAndGetStickerSet(const boost::property_tree::ptree& data) const;
  std::string parseStickerSet(const StickerSet::Ptr& object) const;

  MaskPosition::Ptr parseJsonAndGetMaskPosition(const boost::property_tree::ptree& data) const;
  std::string parseMaskPosition(const MaskPosition::Ptr& object) const;

  InputSticker::Ptr parseJsonAndGetInputSticker(const boost::property_tree::ptree& data) const;
  std::string parseInputSticker(const InputSticker::Ptr& object) const;

  InlineQuery::Ptr parseJsonAndGetInlineQuery(const boost::property_tree::ptree& data) const;
  std::string parseInlineQuery(const InlineQuery::Ptr& object) const;

  InlineQueryResultsButton::Ptr parseJsonAndGetInlineQueryResultsButton(
      const boost::property_tree::ptree& data) const;
  std::string parseInlineQueryResultsButton(const InlineQueryResultsButton::Ptr& object) const;

  InlineQueryResult::Ptr parseJsonAndGetInlineQueryResult(const boost::property_tree::ptree& data) const;
  std::string parseInlineQueryResult(const InlineQueryResult::Ptr& object) const;

  InlineQueryResultArticle::Ptr parseJsonAndGetInlineQueryResultArticle(
      const boost::property_tree::ptree& data) const;
  std::string parseInlineQueryResultArticle(const InlineQueryResultArticle::Ptr& object) const;

  InlineQueryResultPhoto::Ptr parseJsonAndGetInlineQueryResultPhoto(
      const boost::property_tree::ptree& data) const;
  std::string parseInlineQueryResultPhoto(const InlineQueryResultPhoto::Ptr& object) const;

  InlineQueryResultGif::Ptr parseJsonAndGetInlineQueryResultGif(
      const boost::property_tree::ptree& data) const;
  std::string parseInlineQueryResultGif(const InlineQueryResultGif::Ptr& object) const;

  InlineQueryResultMpeg4Gif::Ptr parseJsonAndGetInlineQueryResultMpeg4Gif(
      const boost::property_tree::ptree& data) const;
  std::string parseInlineQueryResultMpeg4Gif(const InlineQueryResultMpeg4Gif::Ptr& object) const;

  InlineQueryResultVideo::Ptr parseJsonAndGetInlineQueryResultVideo(
      const boost::property_tree::ptree& data) const;
  std::string parseInlineQueryResultVideo(const InlineQueryResultVideo::Ptr& object) const;

  InlineQueryResultAudio::Ptr parseJsonAndGetInlineQueryResultAudio(
      const boost::property_tree::ptree& data) const;
  std::string parseInlineQueryResultAudio(const InlineQueryResultAudio::Ptr& object) const;

  InlineQueryResultVoice::Ptr parseJsonAndGetInlineQueryResultVoice(
      const boost::property_tree::ptree& data) const;
  std::string parseInlineQueryResultVoice(const InlineQueryResultVoice::Ptr& object) const;

  InlineQueryResultDocument::Ptr parseJsonAndGetInlineQueryResultDocument(
      const boost::property_tree::ptree& data) const;
  std::string parseInlineQueryResultDocument(const InlineQueryResultDocument::Ptr& object) const;

  InlineQueryResultLocation::Ptr parseJsonAndGetInlineQueryResultLocation(
      const boost::property_tree::ptree& data) const;
  std::string parseInlineQueryResultLocation(const InlineQueryResultLocation::Ptr& object) const;

  InlineQueryResultVenue::Ptr parseJsonAndGetInlineQueryResultVenue(
      const boost::property_tree::ptree& data) const;
  std::string parseInlineQueryResultVenue(const InlineQueryResultVenue::Ptr& object) const;

  InlineQueryResultContact::Ptr parseJsonAndGetInlineQueryResultContact(
      const boost::property_tree::ptree& data) const;
  std::string parseInlineQueryResultContact(const InlineQueryResultContact::Ptr& object) const;

  InlineQueryResultGame::Ptr parseJsonAndGetInlineQueryResultGame(
      const boost::property_tree::ptree& data) const;
  std::string parseInlineQueryResultGame(const InlineQueryResultGame::Ptr& object) const;

  InlineQueryResultCachedPhoto::Ptr parseJsonAndGetInlineQueryResultCachedPhoto(
      const boost::property_tree::ptree& data) const;
  std::string parseInlineQueryResultCachedPhoto(const InlineQueryResultCachedPhoto::Ptr& object) const;

  InlineQueryResultCachedGif::Ptr parseJsonAndGetInlineQueryResultCachedGif(
      const boost::property_tree::ptree& data) const;
  std::string parseInlineQueryResultCachedGif(const InlineQueryResultCachedGif::Ptr& object) const;

  InlineQueryResultCachedMpeg4Gif::Ptr parseJsonAndGetInlineQueryResultCachedMpeg4Gif(
      const boost::property_tree::ptree& data) const;
  std::string parseInlineQueryResultCachedMpeg4Gif(const InlineQueryResultCachedMpeg4Gif::Ptr& object) const;

  InlineQueryResultCachedSticker::Ptr parseJsonAndGetInlineQueryResultCachedSticker(
      const boost::property_tree::ptree& data) const;
  std::string parseInlineQueryResultCachedSticker(const InlineQueryResultCachedSticker::Ptr& object) const;

  InlineQueryResultCachedDocument::Ptr parseJsonAndGetInlineQueryResultCachedDocument(
      const boost::property_tree::ptree& data) const;
  std::string parseInlineQueryResultCachedDocument(const InlineQueryResultCachedDocument::Ptr& object) const;

  InlineQueryResultCachedVideo::Ptr parseJsonAndGetInlineQueryResultCachedVideo(
      const boost::property_tree::ptree& data) const;
  std::string parseInlineQueryResultCachedVideo(const InlineQueryResultCachedVideo::Ptr& object) const;

  InlineQueryResultCachedVoice::Ptr parseJsonAndGetInlineQueryResultCachedVoice(
      const boost::property_tree::ptree& data) const;
  std::string parseInlineQueryResultCachedVoice(const InlineQueryResultCachedVoice::Ptr& object) const;

  InlineQueryResultCachedAudio::Ptr parseJsonAndGetInlineQueryResultCachedAudio(
      const boost::property_tree::ptree& data) const;
  std::string parseInlineQueryResultCachedAudio(const InlineQueryResultCachedAudio::Ptr& object) const;

  InputMessageContent::Ptr parseJsonAndGetInputMessageContent(const boost::property_tree::ptree& data) const;
  std::string parseInputMessageContent(const InputMessageContent::Ptr& object) const;

  InputTextMessageContent::Ptr parseJsonAndGetInputTextMessageContent(
      const boost::property_tree::ptree& data) const;
  std::string parseInputTextMessageContent(const InputTextMessageContent::Ptr& object) const;

  InputLocationMessageContent::Ptr parseJsonAndGetInputLocationMessageContent(
      const boost::property_tree::ptree& data) const;
  std::string parseInputLocationMessageContent(const InputLocationMessageContent::Ptr& object) const;

  InputVenueMessageContent::Ptr parseJsonAndGetInputVenueMessageContent(
      const boost::property_tree::ptree& data) const;
  std::string parseInputVenueMessageContent(const InputVenueMessageContent::Ptr& object) const;

  InputContactMessageContent::Ptr parseJsonAndGetInputContactMessageContent(
      const boost::property_tree::ptree& data) const;
  std::string parseInputContactMessageContent(const InputContactMessageContent::Ptr& object) const;

  InputInvoiceMessageContent::Ptr parseJsonAndGetInputInvoiceMessageContent(
      const boost::property_tree::ptree& data) const;
  std::string parseInputInvoiceMessageContent(const InputInvoiceMessageContent::Ptr& object) const;

  ChosenInlineResult::Ptr parseJsonAndGetChosenInlineResult(const boost::property_tree::ptree& data) const;
  std::string parseChosenInlineResult(const ChosenInlineResult::Ptr& object) const;

  SentWebAppMessage::Ptr parseJsonAndGetSentWebAppMessage(const boost::property_tree::ptree& data) const;
  std::string parseSentWebAppMessage(const SentWebAppMessage::Ptr& object) const;

  LabeledPrice::Ptr parseJsonAndGetLabeledPrice(const boost::property_tree::ptree& data) const;
  std::string parseLabeledPrice(const LabeledPrice::Ptr& object) const;

  Invoice::Ptr parseJsonAndGetInvoice(const boost::property_tree::ptree& data) const;
  std::string parseInvoice(const Invoice::Ptr& object) const;

  ShippingAddress::Ptr parseJsonAndGetShippingAddress(const boost::property_tree::ptree& data) const;
  std::string parseShippingAddress(const ShippingAddress::Ptr& object) const;

  OrderInfo::Ptr parseJsonAndGetOrderInfo(const boost::property_tree::ptree& data) const;
  std::string parseOrderInfo(const OrderInfo::Ptr& object) const;

  ShippingOption::Ptr parseJsonAndGetShippingOption(const boost::property_tree::ptree& data) const;
  std::string parseShippingOption(const ShippingOption::Ptr& object) const;

  SuccessfulPayment::Ptr parseJsonAndGetSuccessfulPayment(const boost::property_tree::ptree& data) const;
  std::string parseSuccessfulPayment(const SuccessfulPayment::Ptr& object) const;

  ShippingQuery::Ptr parseJsonAndGetShippingQuery(const boost::property_tree::ptree& data) const;
  std::string parseShippingQuery(const ShippingQuery::Ptr& object) const;

  PreCheckoutQuery::Ptr parseJsonAndGetPreCheckoutQuery(const boost::property_tree::ptree& data) const;
  std::string parsePreCheckoutQuery(const PreCheckoutQuery::Ptr& object) const;

  PassportData::Ptr parseJsonAndGetPassportData(const boost::property_tree::ptree& data) const;
  std::string parsePassportData(const PassportData::Ptr& object) const;

  PassportFile::Ptr parseJsonAndGetPassportFile(const boost::property_tree::ptree& data) const;
  std::string parsePassportFile(const PassportFile::Ptr& object) const;

  EncryptedPassportElement::Ptr parseJsonAndGetEncryptedPassportElement(
      const boost::property_tree::ptree& data) const;
  std::string parseEncryptedPassportElement(const EncryptedPassportElement::Ptr& object) const;

  EncryptedCredentials::Ptr parseJsonAndGetEncryptedCredentials(
      const boost::property_tree::ptree& data) const;
  std::string parseEncryptedCredentials(const EncryptedCredentials::Ptr& object) const;

  PassportElementError::Ptr parseJsonAndGetPassportElementError(
      const boost::property_tree::ptree& data) const;
  std::string parsePassportElementError(const PassportElementError::Ptr& object) const;

  PassportElementErrorDataField::Ptr parseJsonAndGetPassportElementErrorDataField(
      const boost::property_tree::ptree& data) const;
  std::string parsePassportElementErrorDataField(const PassportElementErrorDataField::Ptr& object) const;

  PassportElementErrorFrontSide::Ptr parseJsonAndGetPassportElementErrorFrontSide(
      const boost::property_tree::ptree& data) const;
  std::string parsePassportElementErrorFrontSide(const PassportElementErrorFrontSide::Ptr& object) const;

  PassportElementErrorReverseSide::Ptr parseJsonAndGetPassportElementErrorReverseSide(
      const boost::property_tree::ptree& data) const;
  std::string parsePassportElementErrorReverseSide(const PassportElementErrorReverseSide::Ptr& object) const;

  PassportElementErrorSelfie::Ptr parseJsonAndGetPassportElementErrorSelfie(
      const boost::property_tree::ptree& data) const;
  std::string parsePassportElementErrorSelfie(const PassportElementErrorSelfie::Ptr& object) const;

  PassportElementErrorFile::Ptr parseJsonAndGetPassportElementErrorFile(
      const boost::property_tree::ptree& data) const;
  std::string parsePassportElementErrorFile(const PassportElementErrorFile::Ptr& object) const;

  PassportElementErrorFiles::Ptr parseJsonAndGetPassportElementErrorFiles(
      const boost::property_tree::ptree& data) const;
  std::string parsePassportElementErrorFiles(const PassportElementErrorFiles::Ptr& object) const;

  PassportElementErrorTranslationFile::Ptr parseJsonAndGetPassportElementErrorTranslationFile(
      const boost::property_tree::ptree& data) const;
  std::string parsePassportElementErrorTranslationFile(
      const PassportElementErrorTranslationFile::Ptr& object) const;

  PassportElementErrorTranslationFiles::Ptr parseJsonAndGetPassportElementErrorTranslationFiles(
      const boost::property_tree::ptree& data) const;
  std::string parsePassportElementErrorTranslationFiles(
      const PassportElementErrorTranslationFiles::Ptr& object) const;

  PassportElementErrorUnspecified::Ptr parseJsonAndGetPassportElementErrorUnspecified(
      const boost::property_tree::ptree& data) const;
  std::string parsePassportElementErrorUnspecified(const PassportElementErrorUnspecified::Ptr& object) const;

  Game::Ptr parseJsonAndGetGame(const boost::property_tree::ptree& data) const;
  std::string parseGame(const Game::Ptr& object) const;

  CallbackGame::Ptr parseJsonAndGetCallbackGame(const boost::property_tree::ptree& data) const;
  std::string parseCallbackGame(const CallbackGame::Ptr& object) const;

  GameHighScore::Ptr parseJsonAndGetGameHighScore(const boost::property_tree::ptree& data) const;
  std::string parseGameHighScore(const GameHighScore::Ptr& object) const;

  GenericReply::Ptr parseJsonAndGetGenericReply(const boost::property_tree::ptree& data) const;
  std::string parseGenericReply(const GenericReply::Ptr& object) const;

  inline boost::property_tree::ptree parseJson(const std::string& json) const {
    boost::property_tree::ptree tree;
    if (json.empty())
      return tree;
    std::istringstream input(json);
    boost::property_tree::read_json(input, tree);
    return tree;
  }

  template <typename T>
  std::shared_ptr<T> tryParseJson(JsonToTgTypeFunc<T> parseFunc, const boost::property_tree::ptree& data,
                                  const std::string& keyName) const {
    auto treeItem = data.find(keyName);
    if (treeItem == data.not_found()) {
      return std::shared_ptr<T>();
    }
    return (this->*parseFunc)(treeItem->second);
  }

  template <typename T>
  std::vector<std::shared_ptr<T>> parseJsonAndGetArray(JsonToTgTypeFunc<T> parseFunc,
                                                       const boost::property_tree::ptree& data) const {
    std::vector<std::shared_ptr<T>> result;
    result.reserve(data.size());
    for (const std::pair<const std::string, boost::property_tree::ptree>& innerTreeItem : data) {
      result.push_back((this->*parseFunc)(innerTreeItem.second));
    }
    return result;
  }

  template <typename T>
  std::vector<T> parseJsonAndGetArray(std::function<T(const boost::property_tree::ptree&)> parseFunc,
                                      const boost::property_tree::ptree& data,
                                      const std::string& keyName) const {
    std::vector<T> result;
    auto treeItem = data.find(keyName);
    if (treeItem == data.not_found()) {
      return result;
    }
    result.reserve(treeItem->second.size());
    for (const std::pair<const std::string, boost::property_tree::ptree>& innerTreeItem : treeItem->second) {
      result.push_back(parseFunc(innerTreeItem.second));
    }
    return result;
  }

  template <typename T>
  std::vector<std::shared_ptr<T>> parseJsonAndGetArray(JsonToTgTypeFunc<T> parseFunc,
                                                       const boost::property_tree::ptree& data,
                                                       const std::string& keyName) const {
    std::vector<std::shared_ptr<T>> result;
    auto treeItem = data.find(keyName);
    if (treeItem == data.not_found()) {
      return result;
    }
    result.reserve(treeItem->second.size());
    for (const std::pair<const std::string, boost::property_tree::ptree>& innerTreeItem : treeItem->second) {
      result.push_back((this->*parseFunc)(innerTreeItem.second));
    }
    return result;
  }

  template <typename T>
  std::vector<std::vector<std::shared_ptr<T>>> parseJsonAndGet2DArray(JsonToTgTypeFunc<T> parseFunc,
                                                                      const boost::property_tree::ptree& data,
                                                                      const std::string& keyName) const {
    std::vector<std::vector<std::shared_ptr<T>>> result;
    auto treeItem = data.find(keyName);
    if (treeItem == data.not_found()) {
      return result;
    }
    result.reserve(treeItem->second.size());
    for (const std::pair<const std::string, boost::property_tree::ptree>& innerTreeItem : treeItem->second) {
      std::vector<std::shared_ptr<T>> innerResult;
      for (const std::pair<const std::string, boost::property_tree::ptree>& innerInnerTreeItem :
           innerTreeItem.second) {
        innerResult.push_back((this->*parseFunc)(innerInnerTreeItem.second));
      }
      result.push_back(innerResult);
    }
    return result;
  }

  template <typename T>
  std::string parseArray(TgTypeToJsonFunc<T> parseFunc,
                         const std::vector<std::shared_ptr<T>>& objects) const {
    if (objects.empty())
      return "";
    std::string result;
    result += '[';
    for (const std::shared_ptr<T>& item : objects) {
      result += (this->*parseFunc)(item);
      result += ',';
    }
    result.erase(result.length() - 1);
    result += ']';
    return result;
  }
  // TODO охё, ещё и std::function... и тут ещё логичнее делать append чем новую строку
  template <typename T>
  std::string parseArray(std::function<T(const T&)> parseFunc, const std::vector<T>& objects) const {
    if (objects.empty())
      return "";
    std::string result;
    result += '[';
    for (const T& item : objects) {
      result += parseFunc(item);  // TODO чёто неясно а здесь чё, предполагается что T это всегда string?
      result += ',';
    }
    // TODO это по сути pop back...
    // да и написание нормальное без удаления лишних запятых уже изобрели (goto..)
    result.erase(result.length() - 1);
    result += ']';
    return result;
  }

  template <typename T>
  std::string parse2DArray(TgTypeToJsonFunc<T> parseFunc,
                           const std::vector<std::vector<std::shared_ptr<T>>>& objects) const {
    if (objects.empty())
      return "";
    std::string result;
    result += '[';
    for (const std::vector<std::shared_ptr<T>>& item : objects) {
      result += parseArray(parseFunc, item);
      result += ',';
    }
    result.erase(result.length() - 1);
    result += ']';
    return result;
  }

 private:
  inline void removeLastComma(std::string& input) const {
    input.erase(input.length() - 1);
  }

  template <typename T>
  inline void appendToJson(std::string& json, const std::string& varName,
                           const std::shared_ptr<T>& value) const {
    if (value == nullptr) {
      return;
    }
    json += '"';
    json += varName;
    json += R"(":)";
    json += value;
    json += ',';
  }

  template <typename T>
  inline void appendToJson(std::string& json, const std::string& varName, const T& value) const {
    json += '"';
    json += varName;
    json += R"(":)";
    json += value;
    json += ',';
  }

  template <typename T>
  inline void appendToJsonNumber(std::string& json, const std::string& varName, const T& value) const {
    json += '"';
    json += varName;
    json += R"(":)";
    json += std::to_string(value);
    json += ',';
  }

  inline void appendToJson(std::string& json, const std::string& varName, const int& value) const {
    appendToJsonNumber(json, varName, value);
  }
  inline void appendToJson(std::string& json, const std::string& varName, const long& value) const {
    appendToJsonNumber(json, varName, value);
  }
  inline void appendToJson(std::string& json, const std::string& varName, const long long& value) const {
    appendToJsonNumber(json, varName, value);
  }
  inline void appendToJson(std::string& json, const std::string& varName, const unsigned& value) const {
    appendToJsonNumber(json, varName, value);
  }
  inline void appendToJson(std::string& json, const std::string& varName, const unsigned long& value) const {
    appendToJsonNumber(json, varName, value);
  }
  inline void appendToJson(std::string& json, const std::string& varName,
                           const unsigned long long& value) const {
    appendToJsonNumber(json, varName, value);
  }
  inline void appendToJson(std::string& json, const std::string& varName, const float& value) const {
    appendToJsonNumber(json, varName, value);
  }
  inline void appendToJson(std::string& json, const std::string& varName, const double& value) const {
    appendToJsonNumber(json, varName, value);
  }
  inline void appendToJson(std::string& json, const std::string& varName, const long double& value) const {
    appendToJsonNumber(json, varName, value);
  }

  inline void appendToJson(std::string& json, const std::string& varName, const bool& value) const {
    json += '"';
    json += varName;
    json += R"(":)";
    json += (value ? "true" : "false");
    json += ',';
  }

  inline void appendToJson(std::string& json, const std::string& varName, const char* value) const {
    if (value != nullptr) {
      std::string strValue(value);
      appendToJson(json, varName, strValue);
    }
  }

  void appendToJson(std::string& json, const std::string& varName, const std::string& value) const;
};
}  // namespace tgbm
