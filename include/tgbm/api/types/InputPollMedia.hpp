#pragma once

#include <tgbm/api/types/all_fwd.hpp>
#include <tgbm/api/types/InputMediaAnimation.hpp>
#include <tgbm/api/types/InputMediaAudio.hpp>
#include <tgbm/api/types/InputMediaDocument.hpp>
#include <tgbm/api/types/InputMediaLivePhoto.hpp>
#include <tgbm/api/types/InputMediaLocation.hpp>
#include <tgbm/api/types/InputMediaPhoto.hpp>
#include <tgbm/api/types/InputMediaVenue.hpp>
#include <tgbm/api/types/InputMediaVideo.hpp>

namespace tgbm::api {

/*This object represents the content of a poll description or a quiz explanation to be sent. It should be one
 * of*/
struct InputPollMedia {
  oneof<InputMediaAnimation, InputMediaAudio, InputMediaDocument, InputMediaLivePhoto, InputMediaLocation,
        InputMediaPhoto, InputMediaVenue, InputMediaVideo>
      data;
  static constexpr std::string_view discriminator = "type";
  enum struct type_e {
    k_mediaanimation,
    k_mediaaudio,
    k_mediadocument,
    k_medialivephoto,
    k_medialocation,
    k_mediaphoto,
    k_mediavenue,
    k_mediavideo,
    nothing,
  };

  static constexpr size_t variant_size = size_t(type_e::nothing);
  type_e type() const {
    return static_cast<type_e>(data.index());
  }
  InputMediaAnimation* get_mediaanimation() noexcept {
    return data.get_if<InputMediaAnimation>();
  }
  const InputMediaAnimation* get_mediaanimation() const noexcept {
    return data.get_if<InputMediaAnimation>();
  }
  InputMediaAudio* get_mediaaudio() noexcept {
    return data.get_if<InputMediaAudio>();
  }
  const InputMediaAudio* get_mediaaudio() const noexcept {
    return data.get_if<InputMediaAudio>();
  }
  InputMediaDocument* get_mediadocument() noexcept {
    return data.get_if<InputMediaDocument>();
  }
  const InputMediaDocument* get_mediadocument() const noexcept {
    return data.get_if<InputMediaDocument>();
  }
  InputMediaLivePhoto* get_medialivephoto() noexcept {
    return data.get_if<InputMediaLivePhoto>();
  }
  const InputMediaLivePhoto* get_medialivephoto() const noexcept {
    return data.get_if<InputMediaLivePhoto>();
  }
  InputMediaLocation* get_medialocation() noexcept {
    return data.get_if<InputMediaLocation>();
  }
  const InputMediaLocation* get_medialocation() const noexcept {
    return data.get_if<InputMediaLocation>();
  }
  InputMediaPhoto* get_mediaphoto() noexcept {
    return data.get_if<InputMediaPhoto>();
  }
  const InputMediaPhoto* get_mediaphoto() const noexcept {
    return data.get_if<InputMediaPhoto>();
  }
  InputMediaVenue* get_mediavenue() noexcept {
    return data.get_if<InputMediaVenue>();
  }
  const InputMediaVenue* get_mediavenue() const noexcept {
    return data.get_if<InputMediaVenue>();
  }
  InputMediaVideo* get_mediavideo() noexcept {
    return data.get_if<InputMediaVideo>();
  }
  const InputMediaVideo* get_mediavideo() const noexcept {
    return data.get_if<InputMediaVideo>();
  }
  static constexpr type_e discriminate(std::string_view val) {
    return string_switch<type_e>(val)
        .case_("animation", type_e::k_mediaanimation)
        .case_("audio", type_e::k_mediaaudio)
        .case_("document", type_e::k_mediadocument)
        .case_("live_photo", type_e::k_medialivephoto)
        .case_("location", type_e::k_medialocation)
        .case_("photo", type_e::k_mediaphoto)
        .case_("venue", type_e::k_mediavenue)
        .case_("video", type_e::k_mediavideo)
        .or_default(type_e::nothing);
  }

  static constexpr decltype(auto) discriminate(std::string_view val, auto&& visitor) {
    if (val == "animation")
      return visitor.template operator()<InputMediaAnimation>();
    if (val == "audio")
      return visitor.template operator()<InputMediaAudio>();
    if (val == "document")
      return visitor.template operator()<InputMediaDocument>();
    if (val == "live_photo")
      return visitor.template operator()<InputMediaLivePhoto>();
    if (val == "location")
      return visitor.template operator()<InputMediaLocation>();
    if (val == "photo")
      return visitor.template operator()<InputMediaPhoto>();
    if (val == "venue")
      return visitor.template operator()<InputMediaVenue>();
    if (val == "video")
      return visitor.template operator()<InputMediaVideo>();
    return visitor.template operator()<void>();
  }

  std::string_view discriminator_now() const noexcept {
    using enum InputPollMedia::type_e;
    switch (type()) {
      case k_mediaanimation:
        return "animation";
      case k_mediaaudio:
        return "audio";
      case k_mediadocument:
        return "document";
      case k_medialivephoto:
        return "live_photo";
      case k_medialocation:
        return "location";
      case k_mediaphoto:
        return "photo";
      case k_mediavenue:
        return "venue";
      case k_mediavideo:
        return "video";
      case nothing:
        return "";
      default:
        unreachable();
    }
  }
};

}  // namespace tgbm::api
