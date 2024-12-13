#pragma once

#include <anyany/anyany.hpp>

#define ANYANY_METHOD(NAME, SIGNATURE)                                                     \
  namespace noexport {                                                                     \
  template <typename CRTP, typename ArgsFn>                                                \
  struct NAME##_do_invoke_and_plugin;                                                      \
                                                                                           \
  template <typename CRTP, typename Ret, typename... Args>                                 \
  struct NAME##_do_invoke_and_plugin<CRTP, Ret(Args...)> {                                 \
    static Ret do_invoke(auto& self, Args&&... args) {                                     \
      return self.NAME(static_cast<Args&&>(args)...);                                      \
    }                                                                                      \
                                                                                           \
    template <typename CRTP2>                                                              \
    struct plugin {                                                                        \
      Ret NAME(Args... args) {                                                             \
        return aa::invoke<CRTP>(static_cast<CRTP2&>(*this), static_cast<Args&&>(args)...); \
      }                                                                                    \
    };                                                                                     \
  };                                                                                       \
  }                                                                                        \
  struct NAME##_m : noexport::NAME##_do_invoke_and_plugin<NAME##_m, SIGNATURE> {};

#define ANYANY_CONST_METHOD(NAME, SIGNATURE)                                                     \
  namespace noexport {                                                                           \
  template <typename CRTP, typename ArgsFn>                                                      \
  struct NAME##_do_invoke_and_plugin;                                                            \
                                                                                                 \
  template <typename CRTP, typename Ret, typename... Args>                                       \
  struct NAME##_do_invoke_and_plugin<CRTP, Ret(Args...)> {                                       \
    static Ret do_invoke(const auto& self, Args&&... args) {                                     \
      return self.NAME(static_cast<Args&&>(args)...);                                            \
    }                                                                                            \
                                                                                                 \
    template <typename CRTP2>                                                                    \
    struct plugin {                                                                              \
      Ret NAME(Args... args) const {                                                             \
        return aa::invoke<CRTP>(static_cast<const CRTP2&>(*this), static_cast<Args&&>(args)...); \
      }                                                                                          \
    };                                                                                           \
  };                                                                                             \
  }                                                                                              \
  struct NAME##_m : noexport::NAME##_do_invoke_and_plugin<NAME##_m, SIGNATURE> {};

#define $inplace(...) ::aa::inplaced([&] { return __VA_ARGS__; })
