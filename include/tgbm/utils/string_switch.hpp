#pragma once

#include <string_view>
#include <optional>
#include <cassert>

namespace tgbm {

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
  constexpr string_switch& operator|(case_t p) {
    return case_(p.key, p.value);
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

}  // namespace tgbm
