#pragma once

#include <compare>
#include <concepts>
#include <memory>
#include <type_traits>
#include <utility>
#include <cassert>

#include <tgbm/utils/macro.hpp>
#include <tgbm/utils/meta.hpp>
#include <tgbm/utils/scope_exit.hpp>

namespace tgbm {

template <typename T>
struct TGBM_TRIVIAL_ABI box {
  static_assert(is_decayed_v<T>);
  using value_type = T;

 private:
  T* ptr = nullptr;

 public:
  box() = default;

  constexpr box(std::nullptr_t) {
  }

  constexpr ~box() {
    reset();
  }

  constexpr box(const box& other) {
    if (!other.ptr)
      return;
    ptr = new T(*other.ptr);
  }
  constexpr box(box&& other) noexcept : ptr(std::exchange(other.ptr, nullptr)) {
  }

  constexpr box& operator=(const box& other) {
    *this = box(other);
    return *this;
  }

  constexpr box& operator=(box&& other) noexcept {
    swap(other);
    return *this;
  }

  constexpr box& operator=(std::nullptr_t) noexcept {
    reset();
    return *this;
  }

  template <typename U = T>
  constexpr box& operator=(U&& v) noexcept(std::is_nothrow_constructible_v<T, U&&>)
    requires(!std::is_same_v<box, std::decay_t<U>> && std::is_constructible_v<T, U &&>)
  {
    emplace(std::forward<U>(v));
    return *this;
  }

  constexpr void swap(box& other) noexcept {
    std::swap(ptr, other.ptr);
  }
  constexpr friend void swap(box& a, box& b) noexcept {
    a.swap(b);
  }

  template <typename U = T,
            std::enable_if_t<std::conjunction_v<std::negation<std::is_same<box, std::decay_t<U>>>,
                                                std::is_constructible<T, U&&>>,
                             int> = 0>
  constexpr explicit(!std::is_convertible_v<U&&, T>)
      box(U&& v) noexcept(std::is_nothrow_constructible_v<T, U&&>)
      : ptr(new T(std::forward<U>(v))) {
  }

  template <typename U, typename... Args>
  constexpr value_type& emplace(std::initializer_list<U> list, Args&&... args) {
    // avoid recursion
    return emplace<std::initializer_list<U>, Args&&...>(std::move(list), std::forward<Args>(args)...);
  }

  // if exception thrown from T constructor, then has_value() == false
  template <typename... Args>
  constexpr T& emplace(Args&&... args) {
    if ((std::is_nothrow_constructible_v<T, Args&&...> || std::is_nothrow_default_constructible_v<T>) &&
        ptr) {
      // reuse memory
      std::destroy_at(ptr);
      on_scope_failure(freemem) {
        // if throw happens, then !std::is_nothrow_constructible_v<T, Args&&...>
        // we are in this branch, this means is_nothrow_default_constructible<T> == true

        // avoid compilation error for non default constructible types,
        // they cannot appear in this branch
        if constexpr (std::is_default_constructible_v<T>) {
          // avoid calling destructor on empty memory by 'delete'
          new (ptr) T;
          reset();
        } else {
          unreachable();
        }
      };
      ptr = new (ptr) T(std::forward<Args>(args)...);
      freemem.no_longer_needed();
    } else {
      ptr = new T(std::forward<Args>(args)...);
    }
    return *ptr;
  }

  [[nodiscard]] bool has_value() const noexcept {
    return ptr != nullptr;
  }

  constexpr explicit operator bool() const noexcept {
    return has_value();
  }

  constexpr void reset() noexcept {
    static_assert(sizeof(T));
    if (ptr) {
      delete ptr;
      ptr = nullptr;
    }
  }

  template <typename U>
  constexpr T value_or(U&& v) const& {
    return has_value() ? T(**this) : static_cast<T>(std::forward<U>(v));
  }

  template <typename U>
  constexpr T value_or(U&& v) && {
    return has_value() ? T(std::move(**this)) : static_cast<T>(std::forward<U>(v));
  }

  constexpr T* operator->() noexcept {
    return ptr;
  }
  constexpr const T* operator->() const noexcept {
    return ptr;
  }

  constexpr T& operator*() & noexcept {
    assert(ptr);
    return *ptr;
  }
  constexpr const T& operator*() const& noexcept {
    assert(ptr);
    return *ptr;
  }
  constexpr T&& operator*() && noexcept {
    assert(ptr);
    return std::move(*ptr);
  }
  constexpr const T&& operator*() const&& noexcept {
    assert(ptr);
    return std::move(*ptr);
  }

  // assumes 'ptr' may be released with 'delete'
  static constexpr box from_raw(T* ptr) noexcept {
    box b;
    b.ptr = ptr;
    return b;
  }

  [[nodiscard]] T* release() noexcept {
    return std::exchange(ptr, nullptr);
  }

  bool operator==(std::nullptr_t) const noexcept {
    return !has_value();
  }
  friend std::strong_ordering operator<=>(const box& l, const box& r) noexcept {
    return l && r ? *l <=> *r : bool(l) <=> bool(r);
  }

  friend bool operator==(const box& l, const box& r) noexcept {
    return l && r ? *l == *r : !l && !r;
  }
};

template <typename T>
box(T&&) -> box<std::decay_t<T>>;

}  // namespace tgbm
