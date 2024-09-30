#pragma once

#include <cassert>
#include <cmath>
#include <cstdint>
#include <compare>

namespace tgbm::api {

struct Double {
  double value = 0;

  constexpr Double() = default;
  constexpr Double(double d) noexcept : value(d) {
    assert(std::isfinite(d));
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
