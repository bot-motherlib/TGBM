#pragma once

#include "all_fwd.hpp"
#include "PaidMediaPreview.hpp"
#include "PaidMediaPhoto.hpp"
#include "PaidMediaVideo.hpp"

namespace tgbm::api {

/*This object describes paid media. Currently, it can be one of*/
struct PaidMedia {
  oneof<PaidMediaPreview, PaidMediaPhoto, PaidMediaVideo> data;
  static constexpr std::string_view discriminator = "type";
  enum struct type_e {
    k_preview,
    k_photo,
    k_video,
    nothing,
  };

  static constexpr size_t variant_size = size_t(type_e::nothing);
  type_e type() const {
    return static_cast<type_e>(data.index());
  }
  PaidMediaPreview* get_preview() noexcept {
    return data.get_if<PaidMediaPreview>();
  }
  const PaidMediaPreview* get_preview() const noexcept {
    return data.get_if<PaidMediaPreview>();
  }
  PaidMediaPhoto* get_photo() noexcept {
    return data.get_if<PaidMediaPhoto>();
  }
  const PaidMediaPhoto* get_photo() const noexcept {
    return data.get_if<PaidMediaPhoto>();
  }
  PaidMediaVideo* get_video() noexcept {
    return data.get_if<PaidMediaVideo>();
  }
  const PaidMediaVideo* get_video() const noexcept {
    return data.get_if<PaidMediaVideo>();
  }
  static constexpr type_e discriminate(std::string_view val) {
    return utils::string_switch<type_e>(val)
        .case_("preview", type_e::k_preview)
        .case_("photo", type_e::k_photo)
        .case_("video", type_e::k_video)
        .or_default(type_e::nothing);
  }

  static constexpr decltype(auto) discriminate(std::string_view val, auto&& visitor) {
    if (val == "preview")
      return visitor.template operator()<PaidMediaPreview>();
    if (val == "photo")
      return visitor.template operator()<PaidMediaPhoto>();
    if (val == "video")
      return visitor.template operator()<PaidMediaVideo>();
    return visitor.template operator()<void>();
  }

  std::string_view discriminator_now() const noexcept {
    using enum PaidMedia::type_e;
    switch (type()) {
      case k_preview:
        return "preview";
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
