#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*This object represents a message.*/
struct Message {
  /* Unique message identifier inside this chat. In specific instances (e.g., message containing a video sent
   * to a big chat), the server might automatically schedule a message instead of sending it immediately. In
   * such cases, this field will be 0 and the relevant message will be unusable until it is actually sent */
  Integer message_id;
  /* Date the message was sent in Unix time. It is always a positive number, representing a valid date. */
  Integer date;
  /* Chat the message belongs to */
  box<Chat> chat;
  /* Optional. Unique identifier of a message thread to which the message belongs; for supergroups only */
  optional<Integer> message_thread_id;
  /* Optional. Sender of the message; may be empty for messages sent to channels. For backward compatibility,
   * if the message was sent on behalf of a chat, the field contains a fake sender user in non-channel chats
   */
  box<User> from;
  /* Optional. Sender of the message when sent on behalf of a chat. For example, the supergroup itself for
   * messages sent by its anonymous administrators or a linked channel for messages automatically forwarded to
   * the channel's discussion group. For backward compatibility, if the message was sent on behalf of a chat,
   * the field from contains a fake sender user in non-channel chats. */
  box<Chat> sender_chat;
  /* Optional. If the sender of the message boosted the chat, the number of boosts added by the user */
  optional<Integer> sender_boost_count;
  /* Optional. The bot that actually sent the message on behalf of the business account. Available only for
   * outgoing messages sent on behalf of the connected business account. */
  box<User> sender_business_bot;
  /* Optional. Unique identifier of the business connection from which the message was received. If non-empty,
   * the message belongs to a chat of the corresponding business account that is independent from any
   * potential bot chat which might share the same identifier. */
  optional<String> business_connection_id;
  /* Optional. Information about the original message for forwarded messages */
  box<MessageOrigin> forward_origin;
  /* Optional. For replies in the same chat and message thread, the original message. Note that the Message
   * object in this field will not contain further reply_to_message fields even if it itself is a reply. */
  box<Message> reply_to_message;
  /* Optional. Information about the message that is being replied to, which may come from another chat or
   * forum topic */
  box<ExternalReplyInfo> external_reply;
  /* Optional. For replies that quote part of the original message, the quoted part of the message */
  box<TextQuote> quote;
  /* Optional. For replies to a story, the original story */
  box<Story> reply_to_story;
  /* Optional. Bot through which the message was sent */
  box<User> via_bot;
  /* Optional. Date the message was last edited in Unix time */
  optional<Integer> edit_date;
  /* Optional. The unique identifier of a media message group this message belongs to */
  optional<String> media_group_id;
  /* Optional. Signature of the post author for messages in channels, or the custom title of an anonymous
   * group administrator */
  optional<String> author_signature;
  /* Optional. The number of Telegram Stars that were paid by the sender of the message to send it */
  optional<Integer> paid_star_count;
  /* Optional. For text messages, the actual UTF-8 text of the message */
  optional<String> text;
  /* Optional. For text messages, special entities like usernames, URLs, bot commands, etc. that appear in the
   * text */
  optional<arrayof<MessageEntity>> entities;
  /* Optional. Options used for link preview generation for the message, if it is a text message and link
   * preview options were changed */
  box<LinkPreviewOptions> link_preview_options;
  /* Optional. Unique identifier of the message effect added to the message */
  optional<String> effect_id;
  /* Optional. Message is an animation, information about the animation. For backward compatibility, when this
   * field is set, the document field will also be set */
  box<Animation> animation;
  /* Optional. Message is an audio file, information about the file */
  box<Audio> audio;
  /* Optional. Message is a general file, information about the file */
  box<Document> document;
  /* Optional. Message contains paid media; information about the paid media */
  box<PaidMediaInfo> paid_media;
  /* Optional. Message is a photo, available sizes of the photo */
  optional<arrayof<PhotoSize>> photo;
  /* Optional. Message is a sticker, information about the sticker */
  box<Sticker> sticker;
  /* Optional. Message is a forwarded story */
  box<Story> story;
  /* Optional. Message is a video, information about the video */
  box<Video> video;
  /* Optional. Message is a video note, information about the video message */
  box<VideoNote> video_note;
  /* Optional. Message is a voice message, information about the file */
  box<Voice> voice;
  /* Optional. Caption for the animation, audio, document, paid media, photo, video or voice */
  optional<String> caption;
  /* Optional. For messages with a caption, special entities like usernames, URLs, bot commands, etc. that
   * appear in the caption */
  optional<arrayof<MessageEntity>> caption_entities;
  /* Optional. Message is a shared contact, information about the contact */
  box<Contact> contact;
  /* Optional. Message is a dice with random value */
  box<Dice> dice;
  /* Optional. Message is a game, information about the game. More about games » */
  box<Game> game;
  /* Optional. Message is a native poll, information about the poll */
  box<Poll> poll;
  /* Optional. Message is a venue, information about the venue. For backward compatibility, when this field is
   * set, the location field will also be set */
  box<Venue> venue;
  /* Optional. Message is a shared location, information about the location */
  box<Location> location;
  /* Optional. New members that were added to the group or supergroup and information about them (the bot
   * itself may be one of these members) */
  optional<arrayof<User>> new_chat_members;
  /* Optional. A member was removed from the group, information about them (this member may be the bot itself)
   */
  box<User> left_chat_member;
  /* Optional. A chat title was changed to this value */
  optional<String> new_chat_title;
  /* Optional. A chat photo was change to this value */
  optional<arrayof<PhotoSize>> new_chat_photo;
  /* Optional. Service message: auto-delete timer settings changed in the chat */
  box<MessageAutoDeleteTimerChanged> message_auto_delete_timer_changed;
  /* Optional. The group has been migrated to a supergroup with the specified identifier. This number may have
   * more than 32 significant bits and some programming languages may have difficulty/silent defects in
   * interpreting it. But it has at most 52 significant bits, so a signed 64-bit integer or double-precision
   * float type are safe for storing this identifier. */
  optional<Integer> migrate_to_chat_id;
  /* Optional. The supergroup has been migrated from a group with the specified identifier. This number may
   * have more than 32 significant bits and some programming languages may have difficulty/silent defects in
   * interpreting it. But it has at most 52 significant bits, so a signed 64-bit integer or double-precision
   * float type are safe for storing this identifier. */
  optional<Integer> migrate_from_chat_id;
  /* Optional. Specified message was pinned. Note that the Message object in this field will not contain
   * further reply_to_message fields even if it itself is a reply. */
  box<MaybeInaccessibleMessage> pinned_message;
  /* Optional. Message is an invoice for a payment, information about the invoice. More about payments » */
  box<Invoice> invoice;
  /* Optional. Message is a service message about a successful payment, information about the payment. More
   * about payments » */
  box<SuccessfulPayment> successful_payment;
  /* Optional. Message is a service message about a refunded payment, information about the payment. More
   * about payments » */
  box<RefundedPayment> refunded_payment;
  /* Optional. Service message: users were shared with the bot */
  box<UsersShared> users_shared;
  /* Optional. Service message: a chat was shared with the bot */
  box<ChatShared> chat_shared;
  /* Optional. Service message: a regular gift was sent or received */
  box<GiftInfo> gift;
  /* Optional. Service message: a unique gift was sent or received */
  box<UniqueGiftInfo> unique_gift;
  /* Optional. The domain name of the website on which the user has logged in. More about Telegram Login » */
  optional<String> connected_website;
  /* Optional. Service message: the user allowed the bot to write messages after adding it to the attachment
   * or side menu, launching a Web App from a link, or accepting an explicit request from a Web App sent by
   * the method requestWriteAccess */
  box<WriteAccessAllowed> write_access_allowed;
  /* Optional. Telegram Passport data */
  box<PassportData> passport_data;
  /* Optional. Service message. A user in the chat triggered another user's proximity alert while sharing Live
   * Location. */
  box<ProximityAlertTriggered> proximity_alert_triggered;
  /* Optional. Service message: user boosted the chat */
  box<ChatBoostAdded> boost_added;
  /* Optional. Service message: chat background set */
  box<ChatBackground> chat_background_set;
  /* Optional. Service message: forum topic created */
  box<ForumTopicCreated> forum_topic_created;
  /* Optional. Service message: forum topic edited */
  box<ForumTopicEdited> forum_topic_edited;
  /* Optional. Service message: forum topic closed */
  box<ForumTopicClosed> forum_topic_closed;
  /* Optional. Service message: forum topic reopened */
  box<ForumTopicReopened> forum_topic_reopened;
  /* Optional. Service message: the 'General' forum topic hidden */
  box<GeneralForumTopicHidden> general_forum_topic_hidden;
  /* Optional. Service message: the 'General' forum topic unhidden */
  box<GeneralForumTopicUnhidden> general_forum_topic_unhidden;
  /* Optional. Service message: a scheduled giveaway was created */
  box<GiveawayCreated> giveaway_created;
  /* Optional. The message is a scheduled giveaway message */
  box<Giveaway> giveaway;
  /* Optional. A giveaway with public winners was completed */
  box<GiveawayWinners> giveaway_winners;
  /* Optional. Service message: a giveaway without public winners was completed */
  box<GiveawayCompleted> giveaway_completed;
  /* Optional. Service message: the price for paid messages has changed in the chat */
  box<PaidMessagePriceChanged> paid_message_price_changed;
  /* Optional. Service message: video chat scheduled */
  box<VideoChatScheduled> video_chat_scheduled;
  /* Optional. Service message: video chat started */
  box<VideoChatStarted> video_chat_started;
  /* Optional. Service message: video chat ended */
  box<VideoChatEnded> video_chat_ended;
  /* Optional. Service message: new participants invited to a video chat */
  box<VideoChatParticipantsInvited> video_chat_participants_invited;
  /* Optional. Service message: data sent by a Web App */
  box<WebAppData> web_app_data;
  /* Optional. Inline keyboard attached to the message. login_url buttons are represented as ordinary url
   * buttons. */
  box<InlineKeyboardMarkup> reply_markup;
  /* Optional. True, if the message is sent to a forum topic */
  optional<True> is_topic_message;
  /* Optional. True, if the message is a channel post that was automatically forwarded to the connected
   * discussion group */
  optional<True> is_automatic_forward;
  /* Optional. True, if the message can't be forwarded */
  optional<True> has_protected_content;
  /* Optional. True, if the message was sent by an implicit action, for example, as an away or a greeting
   * business message, or as a scheduled message */
  optional<True> is_from_offline;
  /* Optional. True, if the caption must be shown above the message media */
  optional<True> show_caption_above_media;
  /* Optional. True, if the message media is covered by a spoiler animation */
  optional<True> has_media_spoiler;
  /* Optional. Service message: the chat photo was deleted */
  optional<True> delete_chat_photo;
  /* Optional. Service message: the group has been created */
  optional<True> group_chat_created;
  /* Optional. Service message: the supergroup has been created. This field can't be received in a message
   * coming through updates, because bot can't be a member of a supergroup when it is created. It can only be
   * found in reply_to_message if someone replies to a very first message in a directly created supergroup. */
  optional<True> supergroup_chat_created;
  /* Optional. Service message: the channel has been created. This field can't be received in a message coming
   * through updates, because bot can't be a member of a channel when it is created. It can only be found in
   * reply_to_message if someone replies to a very first message in a channel. */
  optional<True> channel_chat_created;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name)
        .case_("message_id", true)
        .case_("date", true)
        .case_("chat", true)
        .or_default(false);
  }
};

}  // namespace tgbm::api
