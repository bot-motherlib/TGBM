#pragma once

#include <limits>
#include <optional>
#include <bit>
#include <cassert>
#include <compare>

#include "tgbm/api/Integer.hpp"
#include "tgbm/tools/meta.hpp"

namespace tgbm::api {

template <typename T>
struct optional {
  static_assert(is_decayed_v<T>);

  using value_type = T;

 private:
  std::optional<T> _val;

 public:
  constexpr optional() noexcept : _val(std::nullopt) {
  }
  constexpr optional(value_type b) noexcept(std::is_nothrow_move_constructible_v<value_type>)
      : _val(std::move(b)) {
  }
  constexpr optional(std::nullopt_t) noexcept : optional() {
  }
  constexpr optional& operator=(std::nullopt_t) noexcept {
    _val = std::nullopt;
    return *this;
  }
  constexpr optional& operator=(value_type b) noexcept(std::is_nothrow_move_assignable_v<value_type>) {
    _val = std::move(b);
    return *this;
  }
  constexpr bool has_value() const noexcept {
    return _val.has_value();
  }
  constexpr explicit operator bool() const noexcept {
    return has_value();
  }
  constexpr void reset() noexcept {
    _val.reset();
  }
  template <typename... Args>
  constexpr value_type& emplace(Args&&... args) noexcept {
    return _val.emplace(std::forward<Args>(args)...);
  }
  constexpr void swap(optional& o) noexcept {
    using std::swap;
    swap(_val, o._val);
  }
  friend constexpr void swap(optional& l, optional& r) noexcept {
    l.swap(r);
  }
  constexpr decltype(auto) value_or(value_type val) const noexcept {
    return _val.value_or(std::move(val));
  }

  constexpr value_type& value() {
    return _val.value();
  }
  constexpr const value_type& value() const {
    return _val.value();
  }

  constexpr value_type* operator->() noexcept {
    return _val.operator->();
  }
  constexpr const value_type* operator->() const noexcept {
    return _val.operator->();
  }
  constexpr value_type& operator*() noexcept {
    return _val.operator*();
  }
  constexpr const value_type& operator*() const noexcept {
    return _val.operator*();
  }

  constexpr bool operator==(std::nullopt_t) const noexcept {
    return !has_value();
  }
  constexpr bool operator==(const optional&) const = default;
  constexpr std::strong_ordering operator<=>(const optional&) const = default;
};

// Note: not empty after move
template <>
struct optional<bool> {
  using value_type = bool;

 private:
  enum : int8_t { trueval = std::bit_cast<int8_t>(true), falseval = std::bit_cast<int8_t>(false), empty = 2 };
  static_assert(empty != trueval && empty != falseval);
  union {
    int8_t _val;
    bool _bval;
  };

 public:
  constexpr optional() noexcept : _val(empty) {
  }
  constexpr optional(std::in_place_t, value_type val) noexcept : optional(val) {
    _val = true;
  }
  constexpr optional(value_type b) noexcept : _val(b) {
  }
  constexpr optional(std::nullopt_t) noexcept : optional() {
  }
  constexpr optional& operator=(std::nullopt_t) noexcept {
    _val = empty;
    return *this;
  }
  constexpr optional& operator=(value_type b) noexcept {
    _val = b;
    return *this;
  }
  constexpr bool has_value() const noexcept {
    return _val != empty;
  }
  constexpr explicit operator bool() const noexcept {
    return has_value();
  }
  constexpr void reset() noexcept {
    _val = empty;
  }
  constexpr value_type& emplace(value_type v = value_type{}) noexcept {
    return _bval = v;
  }
  constexpr void swap(optional& o) noexcept {
    std::swap(_val, o._val);
  }
  friend constexpr void swap(optional& l, optional& r) noexcept {
    l.swap(r);
  }
  constexpr value_type value_or(value_type val) const noexcept {
    return has_value() ? _val : val;
  }

  constexpr value_type& value() {
    if (!has_value())
      throw std::bad_optional_access();
    return **this;
  }
  constexpr const value_type& value() const {
    if (!has_value())
      throw std::bad_optional_access();
    return **this;
  }

  constexpr value_type* operator->() noexcept {
    assert(has_value());
    return &_bval;
  }
  constexpr const value_type* operator->() const noexcept {
    return &_bval;
  }
  constexpr value_type& operator*() noexcept {
    assert(has_value());
    return _bval;
  }
  constexpr const value_type& operator*() const noexcept {
    assert(has_value());
    return _bval;
  }

  constexpr bool operator==(const optional& other) const noexcept {
    return _val == other._val;
  }
  constexpr bool operator==(std::nullopt_t) const noexcept {
    return !has_value();
  }

  friend constexpr std::strong_ordering operator<=>(const optional& lhs, const optional& rhs) noexcept {
    if (lhs && rhs) {
      return *lhs <=> *rhs;
    }
    if (lhs) {
      return std::strong_ordering::greater;
    }
    if (rhs) {
      return std::strong_ordering::less;
    }
    return std::strong_ordering::equal;
  }
};

template <typename T>
  requires(std::is_empty_v<T>)
struct optional<T> {
  using value_type = T;

  static_assert(is_decayed_v<T>);
  static inline constinit T _dummy;

 private:
  bool _val = false;

 public:
  constexpr optional() noexcept = default;
  constexpr optional(value_type) noexcept {
    _val = true;
  }
  template <typename... Args>
  constexpr optional(std::in_place_t, Args... args) noexcept {
    (void)value_type(std::forward<Args>(args)...);
    _val = true;
  }
  constexpr optional(std::nullopt_t) noexcept : optional() {
  }
  constexpr optional& operator=(std::nullopt_t) noexcept {
    _val = false;
    return *this;
  }
  constexpr optional& operator=(value_type b) noexcept {
    _val = b;
    return *this;
  }
  constexpr bool has_value() const noexcept {
    return _val;
  }
  constexpr explicit operator bool() const noexcept {
    return has_value();
  }
  constexpr void reset() noexcept {
    _val = false;
  }
  constexpr value_type& emplace(auto&&...) noexcept {
    _val = true;
    return _dummy;
  }
  constexpr void swap(optional& o) noexcept {
    std::swap(_val, o._val);
  }
  friend constexpr void swap(optional& l, optional& r) noexcept {
    l.swap(r);
  }
  constexpr value_type value_or(value_type val) const noexcept {
    return has_value() ? _val : val;
  }
  constexpr value_type* operator->() noexcept {
    assert(has_value());
    return std::addressof(_dummy);
  }
  constexpr const value_type* operator->() const noexcept {
    return std::addressof(_dummy);
  }
  constexpr value_type& operator*() noexcept {
    assert(has_value());
    return _dummy;
  }
  constexpr const value_type& operator*() const noexcept {
    assert(has_value());
    return _dummy;
  }
  constexpr value_type& value() {
    if (!has_value())
      throw std::bad_optional_access();
    return _dummy;
  }
  constexpr const value_type& value() const {
    if (!has_value())
      throw std::bad_optional_access();
    return _dummy;
  }
  constexpr bool operator==(const optional& other) const noexcept {
    return _val == other._val;
  }
  constexpr bool operator==(std::nullopt_t) const noexcept {
    return !has_value();
  }
  friend constexpr std::strong_ordering operator<=>(const optional& lhs, const optional& rhs) noexcept {
    if (lhs && rhs) {
      return *lhs <=> *rhs;
    }
    if (lhs) {
      return std::strong_ordering::greater;
    }
    if (rhs) {
      return std::strong_ordering::less;
    }
    return std::strong_ordering::equal;
  }
};

template <>
struct optional<Integer> {
  using value_type = Integer;

 private:
  static constexpr Integer empty_ = std::numeric_limits<int64_t>::max(); 
  Integer _val = empty_;

 public:
  constexpr optional() noexcept = default;
  constexpr optional(value_type val) noexcept : _val(val) {
  }
  constexpr optional(std::nullopt_t) noexcept : optional() {
  }
  constexpr optional& operator=(std::nullopt_t) noexcept {
    _val = empty_;
    return *this;
  }
  constexpr optional& operator=(value_type b) noexcept {
    _val = b;
    return *this;
  }
  constexpr bool has_value() const noexcept {
    return _val != empty_;
  }
  constexpr explicit operator bool() const noexcept {
    return has_value();
  }
  constexpr void reset() noexcept {
    _val = empty_;
  }
  constexpr value_type& emplace(value_type v = value_type{}) noexcept {
    return _val = v;
  }
  constexpr void swap(optional& o) noexcept {
    std::swap(_val, o._val);
  }
  friend constexpr void swap(optional& l, optional& r) noexcept {
    l.swap(r);
  }
  constexpr value_type value_or(value_type val) const noexcept {
    return has_value() ? _val : val;
  }
  constexpr value_type* operator->() noexcept {
    return &_val;
  }
  constexpr const value_type* operator->() const noexcept {
    return &_val;
  }
  constexpr value_type& operator*() noexcept {
    assert(has_value());
    return _val;
  }
  constexpr const value_type& operator*() const noexcept {
    assert(has_value());
    return _val;
  }
  constexpr value_type& value() {
    if (!has_value())
      throw std::bad_optional_access();
    return _val;
  }
  constexpr const value_type& value() const {
    if (!has_value())
      throw std::bad_optional_access();
    return _val;
  }
  constexpr bool operator==(const optional& other) const noexcept {
    return _val == other._val;
  }
  constexpr bool operator==(std::nullopt_t) const noexcept {
    return !has_value();
  }
  friend constexpr std::strong_ordering operator<=>(const optional& l, const optional& r) noexcept {
    if (!l && !r)
      return std::strong_ordering::equal;
    if (!l)
      return std::strong_ordering::less;
    if (!r)
      return std::strong_ordering::greater;
    return *l <=> *r;
  }
};

}  // namespace tgbm::api
