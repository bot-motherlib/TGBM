#pragma once

#include <chrono>
#include <compare>

namespace tgbm {

using duration_t = std::chrono::steady_clock::duration;

struct deadline_t {
  using time_point_t = std::chrono::steady_clock::time_point;

  time_point_t tp;

  [[nodiscard]] constexpr bool is_reached(
      time_point_t point = std::chrono::steady_clock::now()) const noexcept {
    return tp <= point;
  }

  static constexpr deadline_t never() noexcept {
    return deadline_t{time_point_t::max()};
  }
  static constexpr deadline_t yesterday() noexcept {
    return deadline_t{time_point_t::min()};
  }
  std::strong_ordering operator<=>(const deadline_t&) const = default;
};
// TODO tests.. Hmm, what if duration overflows bottom?
inline deadline_t deadline_after(duration_t duration) noexcept {
  // avoid overflow
  auto tp = std::chrono::steady_clock::now();
  if (tp.max() - tp <= duration)
    return deadline_t{tp.max()};
  return deadline_t{tp + duration};
}

}  // namespace tgbm
