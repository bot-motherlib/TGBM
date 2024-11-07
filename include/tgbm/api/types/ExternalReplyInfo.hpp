#pragma once

#include "all_fwd.hpp"

namespace tgbm::api {

/*This object contains information about a message that is being replied to, which may come from another chat
 * or forum topic.*/
struct ExternalReplyInfo {
  /* Origin of the message replied to by the given message */
  box<MessageOrigin> origin;
  /* Optional. Chat the original message belongs to. Available only if the chat is a supergroup or a channel.
   */
  box<Chat> chat;
  /* Optional. Unique message identifier inside the original chat. Available only if the original chat is a
   * supergroup or a channel. */
  optional<Integer> message_id;
  /* Optional. Options used for link preview generation for the original message, if it is a text message */
  box<LinkPreviewOptions> link_preview_options;
  /* Optional. Message is an animation, information about the animation */
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
  /* Optional. Message is a shared contact, information about the contact */
  box<Contact> contact;
  /* Optional. Message is a dice with random value */
  box<Dice> dice;
  /* Optional. Message is a game, information about the game. More about games » */
  box<Game> game;
  /* Optional. Message is a scheduled giveaway, information about the giveaway */
  box<Giveaway> giveaway;
  /* Optional. A giveaway with public winners was completed */
  box<GiveawayWinners> giveaway_winners;
  /* Optional. Message is an invoice for a payment, information about the invoice. More about payments » */
  box<Invoice> invoice;
  /* Optional. Message is a shared location, information about the location */
  box<Location> location;
  /* Optional. Message is a native poll, information about the poll */
  box<Poll> poll;
  /* Optional. Message is a venue, information about the venue */
  box<Venue> venue;
  /* Optional. True, if the message media is covered by a spoiler animation */
  optional<True> has_media_spoiler;

  consteval static bool is_mandatory_field(std::string_view name) {
    return utils::string_switch<bool>(name).case_("origin", true).or_default(false);
  }
};

}  // namespace tgbm::api
