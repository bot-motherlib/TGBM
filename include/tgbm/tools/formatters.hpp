#pragma once

#include <tgbm/tools/traits.hpp>

#include <fmt/format.h>
#include <string_view>
#include <tgbm/tools/pfr_extension.hpp>
#include <tgbm/api/common.hpp>
#include <tgbm/tools/meta.hpp>

namespace fmt {
template <tgbm::aggregate T>
struct formatter<T> : formatter<std::string_view> {
  format_context::iterator format(const T& t, fmt::format_context& ctx) const {
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
  format_context::iterator format(const T& t, fmt::format_context& ctx) const {
    auto out = ctx.out();
    out = fmt::format_to(out, "{}: [{}]", T::discriminator, to_string_view(t.type()));
    t.data.visit_ptr(tgbm::matcher{[&](auto field) {
                                     if (field) {
                                       out = fmt::format_to(out, ", {}", *field);
                                     } else {
                                       out = fmt::format_to(out, ", <empty>");
                                     }
                                   },
                                   [](tgbm::nothing_t) {}});
    return out;
  }
};

template <typename T>
struct formatter<tgbm::box<T>> : formatter<std::string_view> {
  format_context::iterator format(const tgbm::box<T>& t, fmt::format_context& ctx) const {
    if (!t) {
      return fmt::format_to(ctx.out(), "{}", "<empty>");
    } else {
      return fmt::format_to(ctx.out(), "{}", *t);
    }
  }
};

template <typename T>
struct formatter<tgbm::api::optional<T>> : formatter<std::string_view> {
  format_context::iterator format(const tgbm::api::optional<T>& t, fmt::format_context& ctx) const {
    if (!t) {
      return fmt::format_to(ctx.out(), "{}", "<empty>");
    } else {
      return fmt::format_to(ctx.out(), "{}", *t);
    }
  }
};

template <>
struct formatter<tgbm::api::Integer> : formatter<std::string_view> {
  format_context::iterator format(const tgbm::api::Integer& t, fmt::format_context& ctx) const {
    return fmt::format_to(ctx.out(), "{}", t.value);
  }
};

template <>
struct formatter<tgbm::api::True> : formatter<std::string_view> {
  format_context::iterator format(const tgbm::api::True& t, fmt::format_context& ctx) const {
    return fmt::format_to(ctx.out(), "{}", "true");
  }
};

template <typename... Ts>
struct formatter<std::variant<Ts...>> : formatter<std::string_view> {
  format_context::iterator format(const std::variant<Ts...>& t, fmt::format_context& ctx) const {
    return std::visit([&](auto& val) { return fmt::format_to(ctx.out(), "{}", val); }, t);
  }
};

template <typename... Ts>
struct formatter<tgbm::box_union<Ts...>> : formatter<std::string_view> {
  format_context::iterator format(const tgbm::box_union<Ts...>& t, fmt::format_context& ctx) const {
    return t.visit_ptr(tgbm::matcher{
        [&](tgbm::nothing_t) { return fmt::format_to(ctx.out(), "{}", "<empty>"); },
        [&](auto val) {
          if (val) {
            return fmt::format_to(ctx.out(), "{}", "<empty>");
          } else {
            return fmt::format_to(ctx.out(), "{}", *val);
          }
        },
    });
  }
};
}  // namespace fmt
