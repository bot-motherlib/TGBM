#pragma once

#if __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunknown-attributes"
#endif

#include <type_traits>
#include <utility>

#if __cplusplus >= 202002L
#define LOGIC_GUARDS_CONSTEXPR_DCTOR constexpr
#else
#define LOGIC_GUARDS_CONSTEXPR_DCTOR
#endif

// both scope_exit and scope_failure use precondition,
// that invoking 'fn' do not dellocates memory where 'scope_exit/failure' exists
// or atleast do not touch memory after deleting(including lambda capture/references to other objects)

namespace tgbm {

// SCOPE EXIT
// invokes 'Foo' when destroyed
// if scope ended with exception and 'Foo' throws exception terminate is called
template <typename Foo>
struct [[nodiscard("name it")]] scope_exit {
  [[no_unique_address]] Foo fn;

  LOGIC_GUARDS_CONSTEXPR_DCTOR
  ~scope_exit() noexcept(noexcept(fn())) {
    (void)fn();
  }
};
template <typename T>
scope_exit(T&&) -> scope_exit<std::decay_t<T>>;

// ON SCOPE FAILURE
// invokes 'Foo' when destroyed and success point not reached
// success point setted by invoking .no_longer_needed()
template <typename Foo>
struct [[nodiscard("name it and set success points")]] scope_failure {
  [[no_unique_address]] Foo fn;
  bool failed = true;

  void operator=(scope_failure&&) = delete;

  LOGIC_GUARDS_CONSTEXPR_DCTOR
  ~scope_failure() noexcept(noexcept(fn())) {
    if (failed)
      (void)fn();
  }
  // should be called where the 'fn' call is no longer needed
  constexpr void no_longer_needed() noexcept {
    failed = false;
  }
  constexpr void use_and_forget() noexcept(noexcept(fn())) {
    failed = false;
    (void)fn();
  }
};
template <typename T>
scope_failure(T&&) -> scope_failure<std::decay_t<T>>;

}  // namespace tgbm

// MACROS on_scope_exit / on_scope_failure(NAME) for easy usage

namespace tgbm::noexport {

// used only in macro
struct maker {
  template <typename T>
  constexpr auto operator+(T&& fn) const noexcept(std::is_nothrow_constructible_v<std::decay_t<T>, T&&>) {
    return scope_exit{std::forward<T>(fn)};
  }
  template <typename T>
  constexpr auto operator-(T&& fn) const noexcept(std::is_nothrow_constructible_v<std::decay_t<T>, T&&>) {
    return scope_failure{std::forward<T>(fn)};
  }
};

}  // namespace tgbm::noexport

#define on_scope_exit                                                                  \
  [[maybe_unused]] const auto LOGIC_GUARDS_CONCAT(scope_exit, __LINE__, __COUNTER__) = \
      ::tgbm::noexport::maker{} + [&]()
#define on_scope_failure(NAME) auto NAME = ::tgbm::noexport::maker{} - [&]()

#define LOGIC_GUARDS_CONCAT_IMPL_EXPAND(a, b, c) a##_##b##_##c
#define LOGIC_GUARDS_CONCAT_IMPL(a, b, c) LOGIC_GUARDS_CONCAT_IMPL_EXPAND(a, b, c)
#define LOGIC_GUARDS_CONCAT(a, b, c) LOGIC_GUARDS_CONCAT_IMPL(a, b, c)

#if __clang__
#pragma clang diagnostic pop
#endif
