#pragma once

#include <tgbm/api/types/all_fwd.hpp>
#include <tgbm/api/types/InputMediaAnimation.hpp>
#include <tgbm/api/types/InputMediaDocument.hpp>
#include <tgbm/api/types/InputMediaAudio.hpp>
#include <tgbm/api/types/InputMediaPhoto.hpp>
#include <tgbm/api/types/InputMediaVideo.hpp>

namespace tgbm::api {

/*This object represents the content of a media message to be sent. It should be one of*/
struct InputMedia {
  oneof<InputMediaAnimation, InputMediaDocument, InputMediaAudio, InputMediaPhoto, InputMediaVideo> data;
  static constexpr std::string_view discriminator = "type";
  enum struct type_e {
    k_animation,
    k_document,
    k_audio,
    k_photo,
    k_video,
    nothing,
  };

  static constexpr size_t variant_size = size_t(type_e::nothing);
  type_e type() const {
    return static_cast<type_e>(data.index());
  }
  InputMediaAnimation* get_animation() noexcept {
    return data.get_if<InputMediaAnimation>();
  }
  const InputMediaAnimation* get_animation() const noexcept {
    return data.get_if<InputMediaAnimation>();
  }
  InputMediaDocument* get_document() noexcept {
    return data.get_if<InputMediaDocument>();
  }
  const InputMediaDocument* get_document() const noexcept {
    return data.get_if<InputMediaDocument>();
  }
  InputMediaAudio* get_audio() noexcept {
    return data.get_if<InputMediaAudio>();
  }
  const InputMediaAudio* get_audio() const noexcept {
    return data.get_if<InputMediaAudio>();
  }
  InputMediaPhoto* get_photo() noexcept {
    return data.get_if<InputMediaPhoto>();
  }
  const InputMediaPhoto* get_photo() const noexcept {
    return data.get_if<InputMediaPhoto>();
  }
  InputMediaVideo* get_video() noexcept {
    return data.get_if<InputMediaVideo>();
  }
  const InputMediaVideo* get_video() const noexcept {
    return data.get_if<InputMediaVideo>();
  }
  static constexpr type_e discriminate(std::string_view val) {
    return string_switch<type_e>(val)
        .case_("animation", type_e::k_animation)
        .case_("document", type_e::k_document)
        .case_("audio", type_e::k_audio)
        .case_("photo", type_e::k_photo)
        .case_("video", type_e::k_video)
        .or_default(type_e::nothing);
  }

  static constexpr decltype(auto) discriminate(std::string_view val, auto&& visitor) {
    if (val == "animation")
      return visitor.template operator()<InputMediaAnimation>();
    if (val == "document")
      return visitor.template operator()<InputMediaDocument>();
    if (val == "audio")
      return visitor.template operator()<InputMediaAudio>();
    if (val == "photo")
      return visitor.template operator()<InputMediaPhoto>();
    if (val == "video")
      return visitor.template operator()<InputMediaVideo>();
    return visitor.template operator()<void>();
  }

  std::string_view discriminator_now() const noexcept {
    using enum InputMedia::type_e;
    switch (type()) {
      case k_animation:
        return "animation";
      case k_document:
        return "document";
      case k_audio:
        return "audio";
      case k_photo:
        return "photo";
      case k_video:
        return "video";
      case nothing:
        return "";
      default:
        unreachable();
    }
  }
};

}  // namespace tgbm::api
