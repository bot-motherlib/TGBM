#pragma once

#include <tgbm/tools/traits.hpp>

#include <fmt/format.h>
#include <string_view>
#include <tgbm/tools/pfr_extension.hpp>
#include <tgbm/api/common.hpp>
#include <tgbm/tools/meta.hpp>

namespace fmt {

template <>
struct formatter<tgbm::nothing_t> : formatter<std::string_view> {
  static auto format(const tgbm::nothing_t&, auto& ctx) -> decltype(ctx.out()) {
    return fmt::format_to(ctx.out(), "<empty>");
  }
};

template <tgbm::aggregate T>
struct formatter<T> : formatter<std::string_view> {
  static auto format(const T& t, auto& ctx) -> decltype(ctx.out()) {
    auto out = ctx.out();
    auto visiter = [&]<typename Info, typename Field>(const Field& field) {
      out = fmt::format_to(out, R"("{}": [{}])", Info::name.AsStringView(), field);
      if constexpr (Info::index + 1 != boost::pfr::tuple_size_v<T>) {
        out = fmt::format_to(out, ",");
      }
    };
    pfr_extension::visit_object(t, visiter);
    return out;
  }
};

template <tgbm::discriminated_api_type T>
struct formatter<T> : formatter<std::string_view> {
  static auto format(const T& t, auto& ctx) -> decltype(ctx.out()) {
    auto out = ctx.out();
    out = fmt::format_to(out, "{}: [{}]", T::discriminator, to_string_view(t.type()));
    auto visitor = [&](const auto& field) { return fmt::format_to(out, ", {}", field); };
    return t.data.visit(visitor);
  }
};

template <typename T>
struct formatter<tgbm::box<T>> : formatter<std::string_view> {
  static auto format(const tgbm::box<T>& t, auto& ctx) -> decltype(ctx.out()) {
    if (!t)
      return fmt::format_to(ctx.out(), "{}", "<empty>");
    else
      return fmt::format_to(ctx.out(), "{}", *t);
  }
};

template <typename T>
struct formatter<tgbm::api::optional<T>> : formatter<std::string_view> {
  static auto format(const tgbm::api::optional<T>& t, auto& ctx) -> decltype(ctx.out()) {
    if (!t)
      return fmt::format_to(ctx.out(), "{}", "<empty>");
    else
      return fmt::format_to(ctx.out(), "{}", *t);
  }
};

template <>
struct formatter<tgbm::api::Integer> : formatter<std::string_view> {
  static auto format(const tgbm::api::Integer& t, auto& ctx) -> decltype(ctx.out()) {
    return fmt::format_to(ctx.out(), "{}", t.value);
  }
};

template <typename... Ts>
struct formatter<std::variant<Ts...>> : formatter<std::string_view> {
  static auto format(const std::variant<Ts...>& t, auto& ctx) -> decltype(ctx.out()) {
    return std::visit([&](auto& val) { return fmt::format_to(ctx.out(), "{}", val); }, t);
  }
};

template <typename... Ts>
struct formatter<tgbm::box_union<Ts...>> : formatter<std::string_view> {
  static auto format(const tgbm::box_union<Ts...>& t, auto& ctx) -> decltype(ctx.out()) {
    return t.visit_ptr([&](const auto& val) { return fmt::format_to(ctx.out(), "{}", val); });
  }
};

}  // namespace fmt
