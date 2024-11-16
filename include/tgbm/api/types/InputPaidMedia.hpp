#pragma once

#include "all_fwd.hpp"
#include "InputPaidMediaPhoto.hpp"
#include "InputPaidMediaVideo.hpp"

namespace tgbm::api {

/*This object describes the paid media to be sent. Currently, it can be one of*/
struct InputPaidMedia {
  oneof<InputPaidMediaPhoto, InputPaidMediaVideo> data;
  static constexpr std::string_view discriminator = "type";
  enum struct type_e {
    k_photo,
    k_video,
    nothing,
  };

  static constexpr size_t variant_size = size_t(type_e::nothing);
  type_e type() const {
    return static_cast<type_e>(data.index());
  }
  InputPaidMediaPhoto* get_photo() noexcept {
    return data.get_if<InputPaidMediaPhoto>();
  }
  const InputPaidMediaPhoto* get_photo() const noexcept {
    return data.get_if<InputPaidMediaPhoto>();
  }
  InputPaidMediaVideo* get_video() noexcept {
    return data.get_if<InputPaidMediaVideo>();
  }
  const InputPaidMediaVideo* get_video() const noexcept {
    return data.get_if<InputPaidMediaVideo>();
  }
  static constexpr type_e discriminate(std::string_view val) {
    return string_switch<type_e>(val)
        .case_("photo", type_e::k_photo)
        .case_("video", type_e::k_video)
        .or_default(type_e::nothing);
  }

  static constexpr decltype(auto) discriminate(std::string_view val, auto&& visitor) {
    if (val == "photo")
      return visitor.template operator()<InputPaidMediaPhoto>();
    if (val == "video")
      return visitor.template operator()<InputPaidMediaVideo>();
    return visitor.template operator()<void>();
  }

  std::string_view discriminator_now() const noexcept {
    using enum InputPaidMedia::type_e;
    switch (type()) {
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
