#pragma once

#include <cassert>
#include <compare>

#include <fmt/format.h>

namespace tgbm::api {
// TODO rename (Float or smth)
struct Double {
  double value = 0;

  constexpr Double() = default;
  constexpr Double(double d) noexcept : value(d) {
  }
  constexpr Double& operator=(double d) noexcept {
    value = d;
    return *this;
  }
  constexpr operator double() const noexcept {
    return value;
  }
  friend std::strong_ordering operator<=>(const Double& lhs, const Double& rhs) {
    return std::strong_order(lhs.value, rhs.value);
  }
  friend std::strong_ordering operator<=>(const Double& lhs, const double& rhs) {
    return std::strong_order(lhs.value, rhs);
  }
};

}  // namespace tgbm::api

namespace fmt {

template <>
struct formatter<tgbm::api::Double> : formatter<double> {
  auto format(tgbm::api::Double val, auto& ctx) const -> decltype(ctx.out()) {
    return formatter<double>::format(val.value, ctx);
  }
};

}  // namespace fmt
