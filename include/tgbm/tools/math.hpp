#pragma once
#include <stdexcept>
#include <tgbm/tools/formatters.hpp>
#include <anyany/noexport/type_descriptor_details.hpp>
#include <tgbm/api/common.hpp>
#include <limits>

namespace tgbm {

template <typename T>
struct numeric_limits {
  static constexpr T min = std::numeric_limits<T>::min();
  static constexpr T max = std::numeric_limits<T>::max();
};

template <>
struct numeric_limits<api::Integer> {
  static constexpr api::Integer min = std::numeric_limits<std::int64_t>::min();
  static constexpr api::Integer max = api::Integer::max;
};

// template <>
// struct numeric_limits<>

template <class R, class T>
constexpr bool in_range(T t) noexcept {
  constexpr auto T_min = numeric_limits<T>::min;
  constexpr auto R_min = numeric_limits<R>::min;

  if constexpr (T_min < R_min) {
    if (t < T(R_min)) {
      return false;
    }
  }

  constexpr auto T_max = numeric_limits<T>::max;
  constexpr auto R_max = numeric_limits<R>::max;

  if constexpr (T_max > R_max) {
    if (t > T(R_max)) {
      return false;
    }
  }

  return true;
}

template <typename To, typename From>
To safe_cast(From from) {
  if (!in_range<To>(from)) {
    throw std::overflow_error(fmt::format("Can't cast [{}] to {}", from, aa::noexport::n<To>()));
  }
  return To(from);
}

template <typename Dst, typename Src>
void safe_write(Dst& dst, Src src) {
  dst = safe_cast<Dst>(src);
}
}  // namespace tgbm
