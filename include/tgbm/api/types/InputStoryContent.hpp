#pragma once

#include <tgbm/api/types/all_fwd.hpp>
#include <tgbm/api/types/InputStoryContentPhoto.hpp>
#include <tgbm/api/types/InputStoryContentVideo.hpp>

namespace tgbm::api {

/*This object describes the content of a story to post. Currently, it can be one of*/
struct InputStoryContent {
  oneof<InputStoryContentPhoto, InputStoryContentVideo> data;
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
  InputStoryContentPhoto* get_photo() noexcept {
    return data.get_if<InputStoryContentPhoto>();
  }
  const InputStoryContentPhoto* get_photo() const noexcept {
    return data.get_if<InputStoryContentPhoto>();
  }
  InputStoryContentVideo* get_video() noexcept {
    return data.get_if<InputStoryContentVideo>();
  }
  const InputStoryContentVideo* get_video() const noexcept {
    return data.get_if<InputStoryContentVideo>();
  }
  static constexpr type_e discriminate(std::string_view val) {
    return string_switch<type_e>(val)
        .case_("photo", type_e::k_photo)
        .case_("video", type_e::k_video)
        .or_default(type_e::nothing);
  }

  static constexpr decltype(auto) discriminate(std::string_view val, auto&& visitor) {
    if (val == "photo")
      return visitor.template operator()<InputStoryContentPhoto>();
    if (val == "video")
      return visitor.template operator()<InputStoryContentVideo>();
    return visitor.template operator()<void>();
  }

  std::string_view discriminator_now() const noexcept {
    using enum InputStoryContent::type_e;
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
