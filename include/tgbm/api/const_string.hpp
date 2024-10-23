#pragma once

#include <atomic>
#include <cassert>
#include <cstring>
#include <string_view>
#include <utility>
#include <compare>

#include "tgbm/tools/macro.hpp"
#include "tgbm/tools/algorithm.hpp"
#include "tgbm/api/optional.hpp"

namespace tgbm {

struct uninitialized_t {
  explicit uninitialized_t() = default;
};

struct TGBM_TRIVIAL_ABI const_string {
 private:
  uintptr_t ptrlen = 0;

  static constexpr size_t ptrmask = alignof(void*) - 1;
  static_assert(ptrmask != 0);
  // 'ptrmask' reserved for optional
  static constexpr size_t max_small_sz = ptrmask - 1;

  friend struct optional_traits<const_string>;

  static constexpr uintptr_t nullopt_state = []() -> uintptr_t {
    char data[sizeof(uintptr_t)] = {};
    data[sizeof(uintptr_t) - 1] = max_small_sz + 1;
    return std::bit_cast<uintptr_t>(data);
  }();

  [[nodiscard]] bool is_nullopt_state() const noexcept {
    return ptrlen == nullopt_state;
  }

  const_string(std::nullopt_t) noexcept : ptrlen(nullopt_state) {
  }

  struct string_impl {
    std::atomic<size_t> ref_count;
    size_t size;
    char data[];
  };

  bool is_big() const noexcept {
    return ptrlen != 0 && get_small_size() == 0;
  }

  string_impl* get_ptr() const noexcept {
    return reinterpret_cast<string_impl*>(ptrlen & ~ptrmask);
  }

  size_t get_small_size() const noexcept {
    return ((const char*)&ptrlen)[sizeof(void*) - 1] & ptrmask;
  }

  [[nodiscard]] char* resize(size_t len) {
    assert(ptrlen == 0);
    if (len <= max_small_sz) {
      // set size
      ((char*)&ptrlen)[sizeof(void*) - 1] = len;
      return reinterpret_cast<char*>(&ptrlen);
    }
    char* buffer = (char*)malloc(sizeof(string_impl) + len);
    string_impl* impl = new (buffer) string_impl(1, len);
    ptrlen = reinterpret_cast<uintptr_t>(impl);
    return impl->data;
  }

 public:
  const_string() = default;

  const_string(size_t count, uninitialized_t) {
    (void)resize(count);
  }

  const_string(size_t count, char sym) {
    memset(resize(count), sym, count);
  }

  const_string(std::string_view str) {
    size_t len = str.size();
    memcpy(resize(len), str.data(), len);
  }

  template <size_t N>
  const_string(const char (&arr)[N]) : const_string(std::string_view(arr)) {
  }
  const_string(const std::string& s) : const_string(std::string_view(s)) {
  }

  const_string(const const_string& other) noexcept {
    ptrlen = other.ptrlen;
    if (other.is_big())
      get_ptr()->ref_count.fetch_add(1, std::memory_order_acq_rel);
  }

  const_string(const_string&& other) noexcept {
    ptrlen = other.ptrlen;
    other.ptrlen = 0;
  }

  const_string& operator=(const const_string& other) noexcept {
    return *this = const_string(other);
  }

  const_string& operator=(const_string&& other) noexcept {
    swap(other);
    return *this;
  }

  ~const_string() {
    reset();
  }

  void reset() noexcept {
    if (is_big()) {
      string_impl* impl = get_ptr();
      if (impl->ref_count.fetch_sub(1, std::memory_order_acq_rel) == 1)
        free(impl);
    }
    ptrlen = 0;
  }

  // caller must guarantee, that there are no other threads which will
  // write/read data from this string
  // Note: also changing content for all string copies
  char* unsafe_data_for_changing() noexcept {
    return is_big() ? get_ptr()->data : reinterpret_cast<char*>(&ptrlen);
  }
  const char* data() const noexcept {
    return const_cast<const_string*>(this)->unsafe_data_for_changing();
  }

  [[nodiscard]] bool empty() const noexcept {
    return size() == 0;
  }
  size_t size() const noexcept {
    return is_big() ? get_ptr()->size : get_small_size();
  }

  std::string_view str() const noexcept {
    return std::string_view(data(), size());
  }

  template <size_t N>
  bool operator==(const char (&arr)[N]) const noexcept {
    return *this == std::string_view(arr);
  }
  bool operator==(const std::string& s) const noexcept {
    return *this == std::string_view(s);
  }
  bool operator==(const const_string& other) const noexcept {
    return str() == other.str();
  }
  bool operator==(std::string_view s) const noexcept {
    return str() == s;
  }

  template <size_t N>
  std::strong_ordering operator<=>(const char (&arr)[N]) const noexcept {
    return *this <=> std::string_view(arr);
  }
  std::strong_ordering operator<=>(const std::string& s) const noexcept {
    return *this <=> std::string_view(s);
  }
  std::strong_ordering operator<=>(const const_string& other) const noexcept {
    return str() <=> other.str();
  }
  std::strong_ordering operator<=>(std::string_view s) const noexcept {
    return str() <=> s;
  }

  void swap(const_string& other) noexcept {
    std::swap(ptrlen, other.ptrlen);
  }
  friend void swap(const_string& l, const_string& r) noexcept {
    l.swap(r);
  }
};

template <>
struct optional_traits<const_string> {
  struct state_type {
    const_string str;
  };

  static state_type make_nullopt_state() noexcept {
    return state_type{const_string(std::nullopt)};
  }
  static constexpr const_string* get_value_ptr(state_type& s) noexcept {
    return &s.str;
  }

  static constexpr void do_swap(state_type& l, state_type& r) noexcept {
    l.str.swap(r.str);
  }

  static constexpr bool is_nullopt_state(const state_type& s) noexcept {
    return s.str.is_nullopt_state();
  }

  static constexpr void mark_as_filled(state_type& s) noexcept {
    // noop
  }
};

}  // namespace tgbm

namespace std {

template <>
struct hash<::tgbm::const_string> {
  size_t operator()(const ::tgbm::const_string s) const noexcept {
    return ::tgbm::fnv_hash_bytes(s.str());
  }
};

}  // namespace std
