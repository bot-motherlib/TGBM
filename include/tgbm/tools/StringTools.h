#pragma once

#include "tgbm/export.h"

#include <cstddef>
#include <vector>
#include <string>
#include <optional>
#include <cassert>
#include <concepts>

#include <fmt/format.h>

/**
 * @ingroup tools
 */
namespace tgbm::utils {

template <std::output_iterator<const char&> O>
O url_encode(char c, O out) {
  static constexpr auto is_legit = [](char c) {
    switch (c) {
      case 'A' ... 'Z':
      case 'a' ... 'z':
      case '0' ... '9':
      case '_':
      case '.':
      case '-':
      case '~':
      case ':':
        return true;
      default:
        return false;
    }
  };
  if (is_legit(c)) {
    *out = c;
    ++out;
  } else
    out = fmt::format_to(out, "%{:02X}", (unsigned)(unsigned char)c);
  return out;
}

/**
 * Splits string to smaller substrings which have between them a delimiter. Resulting substrings won't have
 * delimiter.
 * @param str Source string
 * @param delimiter Delimiter
 * @param dest Array to which substrings will be saved.
 */
TGBM_API
void split(const std::string& str, char delimiter, std::vector<std::string>& dest);

/**
 * Generates pseudo random string. It's recommended to call srand before this method.
 * @param length Length of resulting string.
 */
TGBM_API
std::string generate_multipart_boundary(std::size_t length);

// used for fmt formatting
struct url_encoded {
  std::string_view str;
};

/**
 * Performs url decode.
 * @param value Encoded url string
 * @return Decoded url string
 */
TGBM_API
std::string urlDecode(const std::string& value);

/**
 * Splits string to smaller substrings which have between them a delimiter. Resulting substrings won't have
 * delimiter.
 * @param str Source string
 * @param delimiter Delimiter
 * @return Array of substrings
 */
inline std::vector<std::string> split(const std::string& str, char delimiter) {
  std::vector<std::string> result;
  split(str, delimiter, result);
  return result;
}

template <typename T, typename R = T>
struct string_switch {
 private:
  // string we are matching
  const std::string_view str;
  // result of this switch statement
  std::optional<T> result;

 public:
  constexpr explicit string_switch(std::string_view S) : str(S), result(std::nullopt) {
  }

  string_switch(const string_switch&) = delete;

  void operator=(const string_switch&) = delete;
  void operator=(string_switch&&) = delete;
#define $$move (T&&)
  constexpr string_switch(string_switch&& other) : str(other.str), result($$move other.result) {
  }

  ~string_switch() = default;

 private:
  constexpr string_switch& elif (std::string_view s, T value) {
    if (!result && str == s)
      result = $$move value;
    return *this;
  }

 public:
  constexpr string_switch& ends_with(std::string_view S, T value) {
    if (!result && str.ends_with(S))
      result = $$move value;
    return *this;
  }

  constexpr string_switch& starts_with(std::string_view S, T value) {
    if (!result && str.starts_with(S))
      result = $$move value;
    return *this;
  }
  constexpr string_switch& case_(std::string_view S0, T value) {
    return elif (S0, value);
  }
  constexpr string_switch& cases(std::string_view S0, T value) {
    return elif (S0, value);
  }
  constexpr string_switch& cases(std::string_view S0, std::string_view S1, T value) {
    return elif (S0, value).elif (S1, value);
  }

  constexpr string_switch& cases(std::string_view S0, std::string_view S1, std::string_view S2, T value) {
    return elif (S0, value).cases(S1, S2, value);
  }

  constexpr string_switch& cases(std::string_view S0, std::string_view S1, std::string_view S2,
                                 std::string_view S3, T value) {
    return elif (S0, value).cases(S1, S2, S3, value);
  }

  constexpr string_switch& cases(std::string_view S0, std::string_view S1, std::string_view S2,
                                 std::string_view S3, std::string_view S4, T value) {
    return elif (S0, value).cases(S1, S2, S3, S4, value);
  }

  constexpr string_switch& cases(std::string_view S0, std::string_view S1, std::string_view S2,
                                 std::string_view S3, std::string_view S4, std::string_view S5, T value) {
    return elif (S0, value).cases(S1, S2, S3, S4, S5, value);
  }

  constexpr string_switch& cases(std::string_view S0, std::string_view S1, std::string_view S2,
                                 std::string_view S3, std::string_view S4, std::string_view S5,
                                 std::string_view S6, T value) {
    return elif (S0, value).cases(S1, S2, S3, S4, S5, S6, value);
  }

  constexpr string_switch& cases(std::string_view S0, std::string_view S1, std::string_view S2,
                                 std::string_view S3, std::string_view S4, std::string_view S5,
                                 std::string_view S6, std::string_view S7, T value) {
    return elif (S0, value).cases(S1, S2, S3, S4, S5, S6, S7, value);
  }

  constexpr string_switch& cases(std::string_view S0, std::string_view S1, std::string_view S2,
                                 std::string_view S3, std::string_view S4, std::string_view S5,
                                 std::string_view S6, std::string_view S7, std::string_view S8, T value) {
    return elif (S0, value).cases(S1, S2, S3, S4, S5, S6, S7, S8, value);
  }

  constexpr string_switch& cases(std::string_view S0, std::string_view S1, std::string_view S2,
                                 std::string_view S3, std::string_view S4, std::string_view S5,
                                 std::string_view S6, std::string_view S7, std::string_view S8,
                                 std::string_view S9, T value) {
    return elif (S0, value).cases(S1, S2, S3, S4, S5, S6, S7, S8, S9, value);
  }

  struct case_t {
    std::string_view key;
    T value;
  };
  // used to expand like
  //   T result = (string_switch(str) | ... | case_t(keys_pack, values_pack)).or_default(val)
  string_switch& operator|(case_t p) {
    Case(p.first, p.second);
    return *this;
  }

  [[nodiscard]] constexpr R or_default(T value) {
    if (result)
      return $$move(*result);
    return value;
  }

  [[nodiscard]] constexpr operator R() {
    assert(result && "Fell off the end of a string-switch");
    return $$move(*result);
  }
#undef $$move
};

}  // namespace tgbm::utils

namespace fmt {

template <>
struct fmt::formatter<::tgbm::utils::url_encoded> {
  static constexpr auto parse(fmt::format_parse_context& ctx) -> decltype(ctx.begin()) {
    return ctx.end();
  }

  static auto format(::tgbm::utils::url_encoded s, auto& ctx) -> decltype(ctx.out()) {
    auto it = ctx.out();
    for (const char& c : s.str)
      it = ::tgbm::utils::url_encode(c, it);
    return it;
  }
};

}  // namespace fmt
