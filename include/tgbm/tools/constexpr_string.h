#pragma once
#include <algorithm>
#include <array>
#include <cstddef>
#include <stdexcept>
#include <string_view>

namespace tgbm::ce {

#ifndef TGBM_FIXED_STRING_SIZE
  #define TGBM_FIXED_STRING_SIZE 64
#endif

constexpr std::size_t k_len_fixed_string = TGBM_FIXED_STRING_SIZE;

struct string {
  constexpr string() noexcept = default;

  constexpr string(const char* c_str) : string(std::string_view{c_str}) {
  }

  template <std::size_t Size>
  constexpr string(const char (&str)[Size]) noexcept : len(Size - 1) {
    static_assert(Size <= capacity);
    std::copy_n(str, Size, data_.begin());
  }

  constexpr string(std::string_view sv) {
    if (sv.size() > k_len_fixed_string) {
      throw std::runtime_error("sv too big");
    }
    std::copy_n(sv.data(), sv.size(), data_.begin());
    len = sv.length();
  }

  constexpr std::string_view AsStringView() const {
    return std::string_view{data_.data(), len};
  }

  constexpr operator std::string_view() const {
    return AsStringView();
  }

  friend constexpr bool operator==(const string& string, const char* rhs) {
    std::string_view right{rhs};
    return string.AsStringView() == right;
  }
  friend constexpr bool operator==(const char* lhs, const string& string) {
    return string == lhs;
  }

  constexpr std::size_t size() const {
    return len;
  }

  constexpr bool empty() const {
    return len == 0;
  }

  constexpr const char* data() const {
    return data_.data();
  }

 public:
  std::size_t len = 0;
  static constexpr std::size_t capacity = k_len_fixed_string;
  std::array<char, capacity> data_{};
};

constexpr string k_empty_fixed_string{};

}  // namespace tgbm::ce
