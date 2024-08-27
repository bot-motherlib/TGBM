#pragma once

#include "tgbm/tools/macro.hpp"

#include <utility>

namespace tgbm {

template <typename T>
inline constexpr bool is_decayed_v = std::is_same_v<T, std::decay_t<T>>;

enum { VISIT_INDEX_MAX = 64 - 1 };
// switch 'i' up to 64 for better code generatrion
// 'F' should be like [] <size_t I> {}
// Max - index until 'f' will be instanciated, assumes i <= MAX
template <size_t Max>
constexpr decltype(auto) visit_index(auto&& f, size_t i) {
  static_assert(Max <= VISIT_INDEX_MAX);
  assert(i <= VISIT_INDEX_MAX);
  if constexpr (Max == 0)
    return;
  switch (i) {
#define $TGBM_SWITCH_CASE(INDEX)             \
  case INDEX: {                              \
    if constexpr (INDEX <= Max) {            \
      return f.template operator()<INDEX>(); \
    } else {                                 \
      KELCORO_UNREACHABLE;                   \
    }                                        \
  }
#define $TGBM_SWITCH_CASE4(I) \
  $TGBM_SWITCH_CASE(I) $TGBM_SWITCH_CASE(I + 1) $TGBM_SWITCH_CASE(I + 2) $TGBM_SWITCH_CASE(I + 3)
#define $TGBM_SWITCH_CASE8(I) $TGBM_SWITCH_CASE4(I) $TGBM_SWITCH_CASE4(I + 4)
#define $TGBM_SWITCH_CASE16(I) $TGBM_SWITCH_CASE8(I) $TGBM_SWITCH_CASE8(I + 8)
#define $TGBM_SWITCH_CASE32(I) $TGBM_SWITCH_CASE16(I) $TGBM_SWITCH_CASE16(I + 16)
#define $TGBM_SWITCH_CASE64(I) $TGBM_SWITCH_CASE32(I) $TGBM_SWITCH_CASE32(I + 32)

    $TGBM_SWITCH_CASE64(0);
  }  // end switch
  KELCORO_UNREACHABLE;
#undef $TGBM_SWITCH_CASE
#undef $TGBM_SWITCH_CASE4
#undef $TGBM_SWITCH_CASE8
#undef $TGBM_SWITCH_CASE16
#undef $TGBM_SWITCH_CASE32
#undef $TGBM_SWITCH_CASE64
}

template <typename... Foos>
struct matcher : Foos... {
  using Foos::operator()...;
};

template <typename... Foos>
matcher(Foos...) -> matcher<Foos...>;

// same as 'matcher', but casts every returned value to type Ret,
// so you dont need duplicate return type on every function
template <typename Ret, typename... Foos>
constexpr auto matcher_r(Foos&&... foos) noexcept(
    (... && std::is_nothrow_constructible_v<std::remove_cvref_t<Foos>, Foos&&>)) {
  return [m = matcher{std::forward<Foos>(foos)...}](auto&&... args) -> Ret {
    // - uses RVO, no move
    // - do not uses std::invoke for better compilation speed/errors
    // and no one literaly needed to pass member pointers here
    return static_cast<Ret>(m(std::forward<decltype(args)>(args)...));
  };
}

consteval size_t log2_constexpr(size_t n) {
  return n == 1 ? 0 : log2_constexpr(n / 2) + 1;
}

template <typename T, typename... Ts>
consteval size_t find_first() {
  size_t result = -1;
  size_t i = -1;
  // for guaranteed sequence
  (void)std::initializer_list<bool>{((++i, std::is_same_v<T, Ts>)&&result == -1 && (result = i))...};
  return result;
}

template <typename T, typename... Ts>
consteval bool contains_type() {
  return find_first<T, Ts...>() != -1;
}

// 'npos' for types
union not_a_type {};

template <typename...>
struct first_type;

template <typename T, typename... Args>
struct first_type<T, Args...> : std::type_identity<T> {};

template <>
struct first_type<> : std::type_identity<not_a_type> {};

template <typename... Ts>
using first_type_t = typename first_type<Ts...>::type;

// no support to void types and I > sizeof...(Types)
template <size_t I, typename... Types>
using element_at_t = std::tuple_element_t<I, std::tuple<Types...>>;

template <typename... Ts>
consteval bool is_unique_types() {
  size_t is[] = {find_first<Ts, Ts...>()..., size_t(-1) /* avoid empty array */};
  auto count = [&is](size_t val) {
    size_t c = 0;
    for (size_t x : is)
      c += val == x;
    return c;
  };
  return ((count(find_first<Ts, Ts...>()) == 1) && ...);
}

template <typename T, typename... Types>
concept oneof = (std::same_as<T, Types> || ...);

}  // namespace tgbm
