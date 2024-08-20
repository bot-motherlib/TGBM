#pragma once

#include <type_traits>
#include <utility>
#include <cassert>

namespace tgbm {

// TODO clang::trivial_abi macro
template <typename T>
struct [[clang::trivial_abi]] box {
 private:
  static_assert(std::is_same_v<T, std::decay_t<T>>);
  T* ptr = nullptr;

 public:
  box() = default;

  constexpr ~box() {
    if (ptr)
      delete ptr;
  }
  constexpr box(T& value) : ptr(new T(value)) {
  }
  constexpr box(const T& value) : ptr(new T(value)) {
  }
  constexpr box(T&& value) : ptr(new T(std::move(value))) {
  }
  constexpr box(const box& other) {
    if (!other.ptr)
      return;
    ptr = new T(*other.ptr);
  }
  constexpr box(box&& other) noexcept(std::is_nothrow_move_constructible_v<T>)
      : ptr(std::exchange(other.ptr, nullptr)) {
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

  constexpr explicit operator bool() const noexcept {
    return ptr != nullptr;
  }
  constexpr T* operator->() noexcept {
    assert(ptr);
    return ptr;
  }
  constexpr T* operator->() const noexcept {
    assert(ptr);
    return ptr;
  }
  [[nodiscard]] constexpr T* get() noexcept {
    return ptr;
  }
  [[nodiscard]] constexpr T* get() const noexcept {
    return ptr;
  }
};

}  // namespace tgbm
