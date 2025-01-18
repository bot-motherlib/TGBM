#pragma once

#include <cstring>
#include <algorithm>
#include <span>
#include <bit>
#include <cassert>

namespace tgbm {

using byte_t = unsigned char;

template <typename T>
std::span<byte_t, sizeof(T)> as_bytes(T& t) noexcept {
  static_assert(std::has_unique_object_representations_v<T> && !std::is_const_v<T> &&
                std::is_trivially_copyable_v<T>);
  return std::span<byte_t, sizeof(T)>(reinterpret_cast<byte_t*>(std::addressof(t)), sizeof(T));
}

template <typename T>
std::span<const byte_t, sizeof(T)> as_bytes(const T& t) noexcept {
  static_assert(std::has_unique_object_representations_v<T> && std::is_trivially_copyable_v<T>);
  return std::span<byte_t, sizeof(T)>(reinterpret_cast<const byte_t*>(std::addressof(t)), sizeof(T));
}
// this is for types, not for spans
template <typename T>
void as_bytes(std::span<T>) = delete;

template <typename T>
std::span<byte_t> reinterpret_span_as_mutable_bytes(std::span<T> f) {
  static_assert(std::has_unique_object_representations_v<T> && std::is_trivially_copyable_v<T> &&
                !std::is_const_v<T>);
  return std::span<byte_t>(reinterpret_cast<byte_t*>(f.data()), f.size_bytes());
}
template <typename T, size_t Sz>
std::span<const byte_t> reinterpret_span_as_bytes(std::span<T, Sz> f) {
  static_assert(std::has_unique_object_representations_v<T> && std::is_trivially_copyable_v<T>);
  return std::span<const byte_t>(reinterpret_cast<const byte_t*>(f.data()), f.size_bytes());
}

template <typename T>
constexpr void byteswap(T& value) noexcept {
  static_assert(std::has_unique_object_representations_v<T>);
  std::span bytes = as_bytes(value);
  std::reverse(bytes.begin(), bytes.end());
}

template <typename T>
[[nodiscard]] constexpr T htonl_copy(T value) noexcept {
  static_assert(std::has_unique_object_representations_v<T> && std::is_trivially_copyable_v<T>);
  using enum std::endian;
  static_assert(native == little || native == big);
  if constexpr (native == little) {
    byteswap(value);
    return value;
  } else {
    return value;
  }
}

// i - in place
template <typename T>
constexpr void htonli(T& value) noexcept {
  static_assert(std::has_unique_object_representations_v<T> && std::is_trivially_copyable_v<T>);
  using enum std::endian;
  static_assert(native == little || native == big);
  if constexpr (native == little) {
    byteswap(value);
    return;
  } else {
    return;
  }
}

template <typename T>
constexpr void remove_prefix(std::span<T>& s, size_t n) noexcept {
  assert(s.size() >= n);
  T* b = s.data() + n;
  T* e = s.data() + s.size();
  s = std::span<T>(b, e);
}

template <typename T>
constexpr void remove_suffix(std::span<T>& s, size_t n) noexcept {
  assert(s.size() >= n);
  T* b = s.data();
  T* e = s.data() + (s.size() - n);
  s = std::span<T>(b, e);
}

template <typename T>
constexpr std::span<T> prefix(std::span<T> s, size_t n) noexcept {
  assert(s.size() >= n);
  T* b = s.data();
  T* e = s.data() + n;
  return std::span<T>(b, e);
}

template <typename T>
constexpr std::span<T> suffix(std::span<T> s, size_t n) noexcept {
  assert(s.size() >= n);
  T* b = s.data() + (s.size() - n);
  T* e = s.data() + s.size();
  return std::span<T>(b, e);
}

// precondition: in and out do not overlap
inline byte_t* copy_bytes(const byte_t* in, size_t sz, byte_t* out) noexcept {
  return (byte_t*)memcpy(out, in, sz) + sz;
}
// precondition: in and out do not overlap
inline byte_t* copy_bytes(std::span<const byte_t> bytes, byte_t* out) noexcept {
  return copy_bytes(bytes.data(), bytes.size(), out);
}

}  // namespace tgbm
