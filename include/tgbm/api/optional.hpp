#pragma once

#include <optional>
#include <bit>
#include <cassert>
#include <compare>
#include <memory>

#include <tgbm/utils/meta.hpp>

namespace tgbm {

template <typename Traits, typename T>
concept optional_traits_for = requires { requires std::is_destructible_v<typename Traits::state_type>; } &&
                              requires(typename Traits::state_type& state) {
                                // may be used to get pointer to uninitialized memory
                                { Traits::get_value_ptr(state) } noexcept -> std::same_as<T*>;
                                { Traits::do_swap(state, state) } -> std::same_as<void>;
                                {
                                  Traits::is_nullopt_state(std::as_const(state))
                                } noexcept -> std::same_as<bool>;
                                // invoked when value is emplaced
                                { Traits::mark_as_filled(state) } -> std::same_as<void>;
                                // destroys value and marks value as nullopt state
                                { Traits::reset(state) } -> std::same_as<void>;
                              };

namespace noexport {

template <typename T>
struct TGBM_TRIVIAL_ABI default_optional_basis {
  union {
    char dummy = {};
    std::remove_const_t<T> value;
  };
  bool has_val = false;

  default_optional_basis()
    requires(std::is_trivially_default_constructible_v<T>)
  = default;
  default_optional_basis(const default_optional_basis&)
    requires(std::is_trivially_copy_constructible_v<T>)
  = default;

  default_optional_basis(default_optional_basis&&)
    requires(std::is_trivially_move_constructible_v<T>)
  = default;
  ~default_optional_basis()
    requires(std::is_trivially_destructible_v<T>)
  = default;

  default_optional_basis& operator=(default_optional_basis&&)
    requires(std::is_trivially_move_assignable_v<T>)
  = default;
  default_optional_basis& operator=(const default_optional_basis&)
    requires(std::is_trivially_copy_assignable_v<T>)
  = default;

  constexpr default_optional_basis() noexcept
    requires(!std::is_trivially_default_constructible_v<T>)
      : dummy(), has_val(false) {
  }
  constexpr default_optional_basis(const default_optional_basis& o) noexcept(
      std::is_nothrow_copy_constructible_v<T>)
    requires(!std::is_trivially_copy_constructible_v<T> && std::is_copy_constructible_v<T>)
      : default_optional_basis() {
    if (o.has_val) {
      std::construct_at(std::addressof(value), o.value);
      has_val = true;
    }
  }
  // ignore exceptions
  constexpr default_optional_basis(default_optional_basis&& o) noexcept
    requires(!std::is_trivially_move_constructible_v<T> && std::is_copy_constructible_v<T>)
      : default_optional_basis() {
    if (o.has_val) {
      std::construct_at(std::addressof(value), std::move(o.value));
      has_val = true;
    }
  }
  constexpr default_optional_basis& operator=(const default_optional_basis& o) noexcept(
      std::is_nothrow_copy_constructible_v<T>)
    requires(!std::is_trivially_copy_assignable_v<T> && std::is_copy_assignable_v<T>)
  {
    *this = default_optional_basis(o);
    return *this;
  }
  // ignore exceptions
  constexpr default_optional_basis& operator=(default_optional_basis&& o) noexcept
    requires(!std::is_trivially_move_assignable_v<T> && std::is_move_assignable_v<T>)
  {
    reset();
    if (o.has_val) {
      std::construct_at(std::addressof(value), std::move(o.value));
      has_val = true;
      o.reset();
    }
    return *this;
  }

  constexpr void reset() noexcept {
    if (has_val) {
      std::destroy_at(std::addressof(value));
      has_val = false;
    }
  }
  constexpr ~default_optional_basis()
    requires(!std::is_trivially_destructible_v<T>)
  {
    reset();
  }
};

}  // namespace noexport

// default implementaton
template <typename T>
struct optional_traits {
  using state_type = noexport::default_optional_basis<T>;

  static constexpr T* get_value_ptr(state_type& s) noexcept {
    return std::addressof(s.value);
  }

  static constexpr void do_swap(state_type& l,
                                state_type& r) noexcept(std::is_nothrow_move_constructible_v<T>) {
    static_assert(std::is_move_constructible_v<T>);

    if constexpr (std::is_trivially_move_constructible_v<T> && std::is_trivially_move_assignable_v<T> &&
                  std::is_trivially_destructible_v<T>) {
      std::swap(l, r);  // memswap
    } else {
      const bool hasval = !is_nullopt_state(l);
      if (hasval == !is_nullopt_state(r)) {
        using std::swap;
        if (hasval)
          swap(*get_value_ptr(l), *get_value_ptr(r));
      } else {
        state_type& src = hasval ? l : r;
        state_type& dst = hasval ? r : l;
        std::construct_at(get_value_ptr(dst), std::move(*get_value_ptr(src)));
        dst.has_val = true;
        src.reset();
      }
    }
  }

  static constexpr void mark_as_filled(state_type& s) noexcept {
    s.has_val = true;
  }
  static constexpr void reset(state_type& s) noexcept {
    s.reset();
  }

  static constexpr bool is_nullopt_state(const state_type& s) noexcept {
    return !s.has_val;
  }
};

template <>
struct optional_traits<bool> {
  struct TGBM_TRIVIAL_ABI state_type {
    union {
      char val;
      bool bval;
    };
  };
  static constexpr inline char empty = 2;

  static constexpr void reset(state_type& s) noexcept {
    s.val = empty;
  }
  static constexpr bool* get_value_ptr(state_type& s) noexcept {
    return std::addressof(s.bval);
  }

  static constexpr void do_swap(state_type& l, state_type& r) noexcept {
    std::swap(l, r);
  }

  static constexpr bool is_nullopt_state(const state_type& s) noexcept {
    assert(std::bit_cast<char>(state_type{.val = empty}) != std::bit_cast<char>(state_type{.bval = true}));
    assert(std::bit_cast<char>(state_type{.val = empty}) != std::bit_cast<char>(state_type{.bval = false}));
    return std::bit_cast<char>(s) == empty;
  }

  static constexpr void mark_as_filled(state_type&) noexcept {
  }
};

template <typename T>
  requires(std::is_empty_v<T> && std::is_trivial_v<T> && std::is_trivially_destructible_v<T>)
struct optional_traits<T> {
  struct TGBM_TRIVIAL_ABI state_type {
    KELCORO_NO_UNIQUE_ADDRESS std::remove_const_t<T> val;
    bool has_val = false;
  };

  static constexpr void reset(state_type& s) noexcept {
    s.has_val = false;
  }
  static constexpr T* get_value_ptr(state_type& s) noexcept {
    return std::addressof(s.val);
  }

  static constexpr void do_swap(state_type& l, state_type& r) noexcept {
    using std::swap;
    swap(l.val, r.val);
    std::swap(l.has_val, r.has_val);
  }

  static constexpr bool is_nullopt_state(const state_type& s) noexcept {
    return !s.has_val;
  }

  static constexpr void mark_as_filled(state_type& s) noexcept {
    s.has_val = true;
  }
};

}  // namespace tgbm

namespace tgbm::api {

template <typename T, optional_traits_for<T> Traits = optional_traits<T>>
struct TGBM_TRIVIAL_ABI optional {
  static_assert(is_decayed_v<T>);

  using value_type = T;

 private:
  using state_t = typename Traits::state_type;
  state_t state;

  [[nodiscard]] constexpr T* value_ptr() noexcept {
    return Traits::get_value_ptr(state);
  }
  [[nodiscard]] constexpr const T* value_ptr() const noexcept {
    return Traits::get_value_ptr(state);
  }

 public:
  constexpr optional() noexcept {
    reset();
  }

  constexpr optional(std::nullopt_t) noexcept : optional() {
  }

  // for basic methods hopes 'state_t' has suitable ctors/assigns
  // this allows optional to be trivial constructible for trivial types

  ~optional() = default;

  optional(const optional&) = default;
  optional(optional&&) = default;

  optional& operator=(optional&&) = default;
  optional& operator=(const optional&) = default;

  constexpr optional& operator=(std::nullopt_t) noexcept {
    reset();
    return *this;
  }

  template <typename U = T>
  constexpr optional& operator=(U&& v) noexcept(std::is_nothrow_constructible_v<T, U&&>)
    requires(!std::is_same_v<optional, std::decay_t<U>> && std::is_constructible_v<T, U &&>)
  {
    emplace(std::forward<U>(v));
    return *this;
  }

  constexpr void swap(optional& o) noexcept {
    using std::swap;
    Traits::do_swap(state, o.state);
  }
  friend constexpr void swap(optional& l, optional& r) noexcept {
    l.swap(r);
  }

  template <typename U = T>
  constexpr explicit(!std::is_convertible_v<U&&, T>)
      optional(U&& v) noexcept(std::is_nothrow_constructible_v<T, U&&>)
    requires(std::is_constructible_v<T, U &&> && !std::is_same_v<optional, std::decay_t<U>>)
  {
    emplace(std::forward<U>(v));
  }

  template <typename U, typename... Args>
  constexpr value_type& emplace(std::initializer_list<U> list, Args&&... args) {
    // avoid recursion
    return emplace<std::initializer_list<U>, Args&&...>(std::move(list), std::forward<Args>(args)...);
  }
  // if exception thrown has_value() == false
  template <typename... Args>
  constexpr value_type& emplace(Args&&... args) noexcept(std::is_nothrow_constructible_v<T, Args&&...>) {
    reset();
    T* p = std::construct_at(value_ptr(), std::forward<Args>(args)...);
    Traits::mark_as_filled(state);
    return *p;
  }

  constexpr bool has_value() const noexcept {
    return !Traits::is_nullopt_state(state);
  }

  constexpr explicit operator bool() const noexcept {
    return has_value();
  }

  constexpr void reset() noexcept {
    Traits::reset(state);
  }

  template <typename U>
  constexpr T value_or(U&& v) const& {
    return has_value() ? T(**this) : static_cast<T>(std::forward<U>(v));
  }
  template <typename U>
  constexpr T value_or(U&& v) && {
    return has_value() ? T(std::move(**this)) : static_cast<T>(std::forward<U>(v));
  }

  constexpr value_type& value() & {
    if (!has_value())
      throw std::bad_optional_access{};
    return **this;
  }
  constexpr const value_type& value() const& {
    if (!has_value())
      throw std::bad_optional_access{};
    return **this;
  }
  constexpr value_type&& value() && {
    if (!has_value())
      throw std::bad_optional_access{};
    return std::move(**this);
  }
  constexpr const value_type&& value() const&& {
    if (!has_value())
      throw std::bad_optional_access{};
    return std::move(**this);
  }

  constexpr value_type* operator->() noexcept {
    return Traits::get_value_ptr(state);
  }
  constexpr const value_type* operator->() const noexcept {
    return const_cast<optional&>(*this).operator->();
  }

  constexpr value_type& operator*() & noexcept {
    assert(has_value());
    return *operator->();
  }
  constexpr const value_type& operator*() const& noexcept {
    assert(has_value());
    return *operator->();
  }
  constexpr value_type&& operator*() && noexcept {
    assert(has_value());
    return std::move(*operator->());
  }
  constexpr const value_type&& operator*() const&& noexcept {
    assert(has_value());
    return std::move(*operator->());
  }

  bool operator==(std::nullopt_t) const noexcept {
    return !has_value();
  }
  constexpr bool operator==(const optional& r) const noexcept {
    const optional& l = *this;
    return l && r ? *l == *r : !l && !r;
  }
  constexpr std::strong_ordering operator<=>(const optional& r) const noexcept {
    const optional& l = *this;
    return l && r ? *l <=> *r : bool(l) <=> bool(r);
  }
};

template <typename T>
optional(T&&) -> optional<std::decay_t<T>>;

}  // namespace tgbm::api
