#pragma once

#include "tgbm/tools/pfr_extension.hpp"
#include "tgbm/tools/box_union.hpp"
#include "tgbm/tools/traits.hpp"

namespace tgbm {
namespace oneof_field_utils {
template <typename T>
constexpr std::size_t N = pfr_extension::tuple_size_v<T> - 1;

template <typename T>
constexpr bool is_required(std::string_view key) {
  return tgbm::call_on_indexes<N<T>>([&]<std::size_t... I> {
    auto ss = tgbm::utils::string_switch<bool>(key);
    using case_t = tgbm::utils::string_switch<bool>::case_t;
    return (ss | ... | case_t(pfr_extension::element_name_v<I, T>, true)).or_default(false);
  });
}

template <oneof_field_api_type T, typename R>
constexpr R emplace_field(auto& data, std::string_view key, auto&& known, auto&& busy, auto&& unknown) {
  static_assert(std::same_as<decltype(T::data), std::remove_cvref_t<decltype(data)>>);
  return T::discriminate_field(key, [&]<typename U>() {
    if constexpr (!std::same_as<U, void>) {
      if (data) {
        return busy();
      }
      auto& u = data.template emplace<U>();
      return known(u.value);
    } else {
      return unknown();
    }
  });
}

template <oneof_field_api_type T>
constexpr void visit(T& t, auto&& visiter, auto&& empty) {
  t.data.visit(tgbm::matcher{[&](nothing_t) { empty(); }, [&](auto& field) { visiter(field.value); }});
}
}  // namespace oneof_field_utils
}  // namespace tgbm
