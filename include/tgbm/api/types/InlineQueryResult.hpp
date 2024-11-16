#pragma once

#include <tgbm/api/types/all_fwd.hpp>
#include <tgbm/api/types/InlineQueryResultCachedAudio.hpp>
#include <tgbm/api/types/InlineQueryResultCachedDocument.hpp>
#include <tgbm/api/types/InlineQueryResultCachedGif.hpp>
#include <tgbm/api/types/InlineQueryResultCachedMpeg4Gif.hpp>
#include <tgbm/api/types/InlineQueryResultCachedPhoto.hpp>
#include <tgbm/api/types/InlineQueryResultCachedSticker.hpp>
#include <tgbm/api/types/InlineQueryResultCachedVideo.hpp>
#include <tgbm/api/types/InlineQueryResultCachedVoice.hpp>
#include <tgbm/api/types/InlineQueryResultArticle.hpp>
#include <tgbm/api/types/InlineQueryResultAudio.hpp>
#include <tgbm/api/types/InlineQueryResultContact.hpp>
#include <tgbm/api/types/InlineQueryResultGame.hpp>
#include <tgbm/api/types/InlineQueryResultDocument.hpp>
#include <tgbm/api/types/InlineQueryResultGif.hpp>
#include <tgbm/api/types/InlineQueryResultLocation.hpp>
#include <tgbm/api/types/InlineQueryResultMpeg4Gif.hpp>
#include <tgbm/api/types/InlineQueryResultPhoto.hpp>
#include <tgbm/api/types/InlineQueryResultVenue.hpp>
#include <tgbm/api/types/InlineQueryResultVideo.hpp>
#include <tgbm/api/types/InlineQueryResultVoice.hpp>

namespace tgbm::api {

/*This object represents one result of an inline query. Telegram clients currently support results of the
 * following 20 types:*/
struct InlineQueryResult {
  oneof<InlineQueryResultCachedAudio, InlineQueryResultCachedDocument, InlineQueryResultCachedGif,
        InlineQueryResultCachedMpeg4Gif, InlineQueryResultCachedPhoto, InlineQueryResultCachedSticker,
        InlineQueryResultCachedVideo, InlineQueryResultCachedVoice, InlineQueryResultArticle,
        InlineQueryResultAudio, InlineQueryResultContact, InlineQueryResultGame, InlineQueryResultDocument,
        InlineQueryResultGif, InlineQueryResultLocation, InlineQueryResultMpeg4Gif, InlineQueryResultPhoto,
        InlineQueryResultVenue, InlineQueryResultVideo, InlineQueryResultVoice>
      data;
  static constexpr std::string_view discriminator = "type";
  enum struct type_e {
    k_cachedaudio,
    k_cacheddocument,
    k_cachedgif,
    k_cachedmpeg4gif,
    k_cachedphoto,
    k_cachedsticker,
    k_cachedvideo,
    k_cachedvoice,
    k_article,
    k_audio,
    k_contact,
    k_game,
    k_document,
    k_gif,
    k_location,
    k_mpeg4gif,
    k_photo,
    k_venue,
    k_video,
    k_voice,
    nothing,
  };

  static constexpr size_t variant_size = size_t(type_e::nothing);
  type_e type() const {
    return static_cast<type_e>(data.index());
  }
  InlineQueryResultCachedAudio* get_cachedaudio() noexcept {
    return data.get_if<InlineQueryResultCachedAudio>();
  }
  const InlineQueryResultCachedAudio* get_cachedaudio() const noexcept {
    return data.get_if<InlineQueryResultCachedAudio>();
  }
  InlineQueryResultCachedDocument* get_cacheddocument() noexcept {
    return data.get_if<InlineQueryResultCachedDocument>();
  }
  const InlineQueryResultCachedDocument* get_cacheddocument() const noexcept {
    return data.get_if<InlineQueryResultCachedDocument>();
  }
  InlineQueryResultCachedGif* get_cachedgif() noexcept {
    return data.get_if<InlineQueryResultCachedGif>();
  }
  const InlineQueryResultCachedGif* get_cachedgif() const noexcept {
    return data.get_if<InlineQueryResultCachedGif>();
  }
  InlineQueryResultCachedMpeg4Gif* get_cachedmpeg4gif() noexcept {
    return data.get_if<InlineQueryResultCachedMpeg4Gif>();
  }
  const InlineQueryResultCachedMpeg4Gif* get_cachedmpeg4gif() const noexcept {
    return data.get_if<InlineQueryResultCachedMpeg4Gif>();
  }
  InlineQueryResultCachedPhoto* get_cachedphoto() noexcept {
    return data.get_if<InlineQueryResultCachedPhoto>();
  }
  const InlineQueryResultCachedPhoto* get_cachedphoto() const noexcept {
    return data.get_if<InlineQueryResultCachedPhoto>();
  }
  InlineQueryResultCachedSticker* get_cachedsticker() noexcept {
    return data.get_if<InlineQueryResultCachedSticker>();
  }
  const InlineQueryResultCachedSticker* get_cachedsticker() const noexcept {
    return data.get_if<InlineQueryResultCachedSticker>();
  }
  InlineQueryResultCachedVideo* get_cachedvideo() noexcept {
    return data.get_if<InlineQueryResultCachedVideo>();
  }
  const InlineQueryResultCachedVideo* get_cachedvideo() const noexcept {
    return data.get_if<InlineQueryResultCachedVideo>();
  }
  InlineQueryResultCachedVoice* get_cachedvoice() noexcept {
    return data.get_if<InlineQueryResultCachedVoice>();
  }
  const InlineQueryResultCachedVoice* get_cachedvoice() const noexcept {
    return data.get_if<InlineQueryResultCachedVoice>();
  }
  InlineQueryResultArticle* get_article() noexcept {
    return data.get_if<InlineQueryResultArticle>();
  }
  const InlineQueryResultArticle* get_article() const noexcept {
    return data.get_if<InlineQueryResultArticle>();
  }
  InlineQueryResultAudio* get_audio() noexcept {
    return data.get_if<InlineQueryResultAudio>();
  }
  const InlineQueryResultAudio* get_audio() const noexcept {
    return data.get_if<InlineQueryResultAudio>();
  }
  InlineQueryResultContact* get_contact() noexcept {
    return data.get_if<InlineQueryResultContact>();
  }
  const InlineQueryResultContact* get_contact() const noexcept {
    return data.get_if<InlineQueryResultContact>();
  }
  InlineQueryResultGame* get_game() noexcept {
    return data.get_if<InlineQueryResultGame>();
  }
  const InlineQueryResultGame* get_game() const noexcept {
    return data.get_if<InlineQueryResultGame>();
  }
  InlineQueryResultDocument* get_document() noexcept {
    return data.get_if<InlineQueryResultDocument>();
  }
  const InlineQueryResultDocument* get_document() const noexcept {
    return data.get_if<InlineQueryResultDocument>();
  }
  InlineQueryResultGif* get_gif() noexcept {
    return data.get_if<InlineQueryResultGif>();
  }
  const InlineQueryResultGif* get_gif() const noexcept {
    return data.get_if<InlineQueryResultGif>();
  }
  InlineQueryResultLocation* get_location() noexcept {
    return data.get_if<InlineQueryResultLocation>();
  }
  const InlineQueryResultLocation* get_location() const noexcept {
    return data.get_if<InlineQueryResultLocation>();
  }
  InlineQueryResultMpeg4Gif* get_mpeg4gif() noexcept {
    return data.get_if<InlineQueryResultMpeg4Gif>();
  }
  const InlineQueryResultMpeg4Gif* get_mpeg4gif() const noexcept {
    return data.get_if<InlineQueryResultMpeg4Gif>();
  }
  InlineQueryResultPhoto* get_photo() noexcept {
    return data.get_if<InlineQueryResultPhoto>();
  }
  const InlineQueryResultPhoto* get_photo() const noexcept {
    return data.get_if<InlineQueryResultPhoto>();
  }
  InlineQueryResultVenue* get_venue() noexcept {
    return data.get_if<InlineQueryResultVenue>();
  }
  const InlineQueryResultVenue* get_venue() const noexcept {
    return data.get_if<InlineQueryResultVenue>();
  }
  InlineQueryResultVideo* get_video() noexcept {
    return data.get_if<InlineQueryResultVideo>();
  }
  const InlineQueryResultVideo* get_video() const noexcept {
    return data.get_if<InlineQueryResultVideo>();
  }
  InlineQueryResultVoice* get_voice() noexcept {
    return data.get_if<InlineQueryResultVoice>();
  }
  const InlineQueryResultVoice* get_voice() const noexcept {
    return data.get_if<InlineQueryResultVoice>();
  }
  static constexpr type_e discriminate(std::string_view val) {
    return string_switch<type_e>(val)
        .case_("audio", type_e::k_cachedaudio)
        .case_("document", type_e::k_cacheddocument)
        .case_("gif", type_e::k_cachedgif)
        .case_("mpeg4_gif", type_e::k_cachedmpeg4gif)
        .case_("photo", type_e::k_cachedphoto)
        .case_("sticker", type_e::k_cachedsticker)
        .case_("video", type_e::k_cachedvideo)
        .case_("voice", type_e::k_cachedvoice)
        .case_("article", type_e::k_article)
        .case_("audio", type_e::k_audio)
        .case_("contact", type_e::k_contact)
        .case_("game", type_e::k_game)
        .case_("document", type_e::k_document)
        .case_("gif", type_e::k_gif)
        .case_("location", type_e::k_location)
        .case_("mpeg4_gif", type_e::k_mpeg4gif)
        .case_("photo", type_e::k_photo)
        .case_("venue", type_e::k_venue)
        .case_("video", type_e::k_video)
        .case_("voice", type_e::k_voice)
        .or_default(type_e::nothing);
  }

  static constexpr decltype(auto) discriminate(std::string_view val, auto&& visitor) {
    if (val == "audio")
      return visitor.template operator()<InlineQueryResultCachedAudio>();
    if (val == "document")
      return visitor.template operator()<InlineQueryResultCachedDocument>();
    if (val == "gif")
      return visitor.template operator()<InlineQueryResultCachedGif>();
    if (val == "mpeg4_gif")
      return visitor.template operator()<InlineQueryResultCachedMpeg4Gif>();
    if (val == "photo")
      return visitor.template operator()<InlineQueryResultCachedPhoto>();
    if (val == "sticker")
      return visitor.template operator()<InlineQueryResultCachedSticker>();
    if (val == "video")
      return visitor.template operator()<InlineQueryResultCachedVideo>();
    if (val == "voice")
      return visitor.template operator()<InlineQueryResultCachedVoice>();
    if (val == "article")
      return visitor.template operator()<InlineQueryResultArticle>();
    if (val == "audio")
      return visitor.template operator()<InlineQueryResultAudio>();
    if (val == "contact")
      return visitor.template operator()<InlineQueryResultContact>();
    if (val == "game")
      return visitor.template operator()<InlineQueryResultGame>();
    if (val == "document")
      return visitor.template operator()<InlineQueryResultDocument>();
    if (val == "gif")
      return visitor.template operator()<InlineQueryResultGif>();
    if (val == "location")
      return visitor.template operator()<InlineQueryResultLocation>();
    if (val == "mpeg4_gif")
      return visitor.template operator()<InlineQueryResultMpeg4Gif>();
    if (val == "photo")
      return visitor.template operator()<InlineQueryResultPhoto>();
    if (val == "venue")
      return visitor.template operator()<InlineQueryResultVenue>();
    if (val == "video")
      return visitor.template operator()<InlineQueryResultVideo>();
    if (val == "voice")
      return visitor.template operator()<InlineQueryResultVoice>();
    return visitor.template operator()<void>();
  }

  std::string_view discriminator_now() const noexcept {
    using enum InlineQueryResult::type_e;
    switch (type()) {
      case k_cachedaudio:
        return "audio";
      case k_cacheddocument:
        return "document";
      case k_cachedgif:
        return "gif";
      case k_cachedmpeg4gif:
        return "mpeg4_gif";
      case k_cachedphoto:
        return "photo";
      case k_cachedsticker:
        return "sticker";
      case k_cachedvideo:
        return "video";
      case k_cachedvoice:
        return "voice";
      case k_article:
        return "article";
      case k_audio:
        return "audio";
      case k_contact:
        return "contact";
      case k_game:
        return "game";
      case k_document:
        return "document";
      case k_gif:
        return "gif";
      case k_location:
        return "location";
      case k_mpeg4gif:
        return "mpeg4_gif";
      case k_photo:
        return "photo";
      case k_venue:
        return "venue";
      case k_video:
        return "video";
      case k_voice:
        return "voice";
      case nothing:
        return "";
      default:
        unreachable();
    }
  }
};

}  // namespace tgbm::api
