#pragma once

#include <compare>
#include <concepts>
#include <type_traits>
#include <utility>
#include <cassert>

#include <tgbm/utils/macro.hpp>

namespace tgbm {

template <typename T>
struct TGBM_TRIVIAL_ABI box {
 private:
  static_assert(std::is_same_v<T, std::decay_t<T>>);

  T* ptr = nullptr;

 public:
  using value_type = T;

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

  template <typename... Args>
  constexpr box(std::in_place_t, Args&&... args) : ptr(new T(std::forward<Args>(args)...)) {
  }
  template <typename... Args>
  constexpr T& emplace(Args&&... args) {
    *this = box(std::in_place, std::forward<Args>(args)...);
    return *ptr;
  }
  constexpr box(T& value) : box(std::in_place, value) {
  }
  constexpr box(const T& value) : box(std::in_place, value) {
  }
  constexpr box(T&& value) : box(std::in_place, std::move(value)) {
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
  constexpr void reset() noexcept {
    static_assert(sizeof(T));
    if (ptr) {
      delete ptr;
      ptr = nullptr;
    }
  }

  constexpr void swap(box& other) noexcept {
    std::swap(ptr, other.ptr);
  }
  constexpr friend void swap(box& a, box& b) noexcept {
    a.swap(b);
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

  constexpr explicit operator bool() const noexcept {
    return ptr != nullptr;
  }
  constexpr T* operator->() noexcept {
    return ptr;
  }
  constexpr T* operator->() const noexcept {
    return ptr;
  }

  [[nodiscard]] constexpr T* get() noexcept {
    return ptr;
  }
  [[nodiscard]] constexpr T* get() const noexcept {
    return ptr;
  }
  constexpr T& operator*() noexcept {
    assert(ptr);
    return *ptr;
  }
  constexpr const T& operator*() const noexcept {
    assert(ptr);
    return *ptr;
  }

  friend std::strong_ordering operator<=>(const box<T>& lhs, const box<T>& rhs) noexcept {
    if (lhs && rhs) {
      return *lhs.ptr <=> *rhs.ptr;
    }
    if (lhs) {
      return std::strong_ordering::greater;
    }
    if (rhs) {
      return std::strong_ordering::less;
    }
    return std::strong_ordering::equal;
  }

  friend bool operator==(const box<T>& lhs, const box<T>& rhs) noexcept {
    if (!lhs && !rhs) {
      return true;
    }
    if (!lhs || !rhs) {
      return false;
    }
    return *lhs.ptr == *rhs.ptr;
  }
};

// TODO? pointer union (llvm) integrated into box

}  // namespace tgbm
