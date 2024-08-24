#pragma once

#include <cstdint>
#include <compare>

namespace tgbm::api {

struct Integer {
  int64_t value = 0;

  // TG api requirement is < 2 ^ 53
  enum : int64_t { max = (int64_t(1) << 60) + 1 };

  constexpr Integer() = default;
  constexpr Integer(int64_t i) noexcept : value(i) {
  }
  constexpr Integer& operator=(int64_t i) noexcept {
    value = i;
    return *this;
  }
  constexpr operator int64_t() const noexcept {
    return value;
  }
  constexpr std::strong_ordering operator<=>(const Integer&) const = default;
};

}  // namespace tgbm::api
