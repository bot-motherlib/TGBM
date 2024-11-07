#pragma once

#include "all_fwd.hpp"
#include "BackgroundFillSolid.hpp"
#include "BackgroundFillGradient.hpp"
#include "BackgroundFillFreeformGradient.hpp"

namespace tgbm::api {

/*This object describes the way a background is filled based on the selected colors. Currently, it can be one
 * of*/
struct BackgroundFill {
  oneof<BackgroundFillSolid, BackgroundFillGradient, BackgroundFillFreeformGradient> data;
  static constexpr std::string_view discriminator = "type";
  enum struct type_e {
    k_solid,
    k_gradient,
    k_freeformgradient,
    nothing,
  };

  static constexpr size_t variant_size = size_t(type_e::nothing);
  type_e type() const {
    return static_cast<type_e>(data.index());
  }
  BackgroundFillSolid* get_solid() noexcept {
    return data.get_if<BackgroundFillSolid>();
  }
  const BackgroundFillSolid* get_solid() const noexcept {
    return data.get_if<BackgroundFillSolid>();
  }
  BackgroundFillGradient* get_gradient() noexcept {
    return data.get_if<BackgroundFillGradient>();
  }
  const BackgroundFillGradient* get_gradient() const noexcept {
    return data.get_if<BackgroundFillGradient>();
  }
  BackgroundFillFreeformGradient* get_freeformgradient() noexcept {
    return data.get_if<BackgroundFillFreeformGradient>();
  }
  const BackgroundFillFreeformGradient* get_freeformgradient() const noexcept {
    return data.get_if<BackgroundFillFreeformGradient>();
  }
  static constexpr type_e discriminate(std::string_view val) {
    return utils::string_switch<type_e>(val)
        .case_("solid", type_e::k_solid)
        .case_("gradient", type_e::k_gradient)
        .case_("freeform_gradient", type_e::k_freeformgradient)
        .or_default(type_e::nothing);
  }

  static constexpr decltype(auto) discriminate(std::string_view val, auto&& visitor) {
    if (val == "solid")
      return visitor.template operator()<BackgroundFillSolid>();
    if (val == "gradient")
      return visitor.template operator()<BackgroundFillGradient>();
    if (val == "freeform_gradient")
      return visitor.template operator()<BackgroundFillFreeformGradient>();
    return visitor.template operator()<void>();
  }

  std::string_view discriminator_now() const noexcept {
    using enum BackgroundFill::type_e;
    switch (type()) {
      case k_solid:
        return "solid";
      case k_gradient:
        return "gradient";
      case k_freeformgradient:
        return "freeform_gradient";
      case nothing:
        return "";
      default:
        unreachable();
    }
  }
};

}  // namespace tgbm::api
