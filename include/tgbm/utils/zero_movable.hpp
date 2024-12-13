#pragma once

#include <utility>

#include <tgbm/utils/macro.hpp>

namespace tgbm {

// for trivial types, which after move must became zero initialized
template <typename T>
struct TGBM_TRIVIAL_ABI zero_movable {
  T value = T{};

  static_assert(std::is_trivial_v<T>);

  zero_movable() = default;

  zero_movable(T v) noexcept : value(std::move(v)) {
  }
  zero_movable(const zero_movable&) = default;
  zero_movable(zero_movable&& other) noexcept : value(std::exchange(other.value, T{})) {
  }

  zero_movable& operator=(const zero_movable&) = default;

  zero_movable& operator=(zero_movable&& other) noexcept {
    value = std::move(other.value);
    other.value = T{};
    return *this;
  }

  ~zero_movable() = default;
};

}  // namespace tgbm
