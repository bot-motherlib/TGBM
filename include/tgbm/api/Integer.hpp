#pragma once

#include <cstdint>
#include <compare>
#include <concepts>
#include <limits>

#include "tgbm/api/optional.hpp"

namespace tgbm::api {

struct Integer {
  int64_t value = 0;

  // TG api requirement is < 2 ^ 53
  static constexpr std::int64_t max = (int64_t(1) << 60) + 1;

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

template <std::integral T>
constexpr std::strong_ordering operator<=>(const Integer& left, T right) {
  return left.value <=> (int64_t)right;
}

}  // namespace tgbm::api

namespace tgbm {

template <>
struct optional_traits<api::Integer> {
  struct state_type {
    api::Integer i;
  };
  // TG requirements for Integer type is to be less then 53 bits long
  static constexpr int64_t empty = std::numeric_limits<int64_t>::max();

  static constexpr state_type make_nullopt_state() noexcept {
    api::Integer i;
    i.value = empty;
    return state_type{i};
  }
  static constexpr api::Integer* get_value_ptr(state_type& s) noexcept {
    return std::addressof(s.i);
  }

  static constexpr void do_swap(state_type& l, state_type& r) noexcept {
    using std::swap;
    swap(l.i.value, r.i.value);
  }

  static constexpr bool is_nullopt_state(const state_type& s) noexcept {
    return s.i.value == empty;
  }

  static constexpr void mark_as_filled(state_type& s) noexcept {
    // noop
  }
};

}  // namespace tgbm
