#pragma once

#include <tgbm/api/types/all_fwd.hpp>
#include <tgbm/api/types/InputProfilePhotoStatic.hpp>
#include <tgbm/api/types/InputProfilePhotoAnimated.hpp>

namespace tgbm::api {

/*This object describes a profile photo to set. Currently, it can be one of*/
struct InputProfilePhoto {
  oneof<InputProfilePhotoStatic, InputProfilePhotoAnimated> data;
  static constexpr std::string_view discriminator = "type";
  enum struct type_e {
    k_static,
    k_animated,
    nothing,
  };

  static constexpr size_t variant_size = size_t(type_e::nothing);
  type_e type() const {
    return static_cast<type_e>(data.index());
  }
  InputProfilePhotoStatic* get_static() noexcept {
    return data.get_if<InputProfilePhotoStatic>();
  }
  const InputProfilePhotoStatic* get_static() const noexcept {
    return data.get_if<InputProfilePhotoStatic>();
  }
  InputProfilePhotoAnimated* get_animated() noexcept {
    return data.get_if<InputProfilePhotoAnimated>();
  }
  const InputProfilePhotoAnimated* get_animated() const noexcept {
    return data.get_if<InputProfilePhotoAnimated>();
  }
  static constexpr type_e discriminate(std::string_view val) {
    return string_switch<type_e>(val)
        .case_("static", type_e::k_static)
        .case_("animated", type_e::k_animated)
        .or_default(type_e::nothing);
  }

  static constexpr decltype(auto) discriminate(std::string_view val, auto&& visitor) {
    if (val == "static")
      return visitor.template operator()<InputProfilePhotoStatic>();
    if (val == "animated")
      return visitor.template operator()<InputProfilePhotoAnimated>();
    return visitor.template operator()<void>();
  }

  std::string_view discriminator_now() const noexcept {
    using enum InputProfilePhoto::type_e;
    switch (type()) {
      case k_static:
        return "static";
      case k_animated:
        return "animated";
      case nothing:
        return "";
      default:
        unreachable();
    }
  }
};

}  // namespace tgbm::api
