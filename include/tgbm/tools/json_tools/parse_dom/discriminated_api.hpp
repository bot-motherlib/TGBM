#pragma once

#include "tgbm/tools/json_tools/parse_dom/basic.hpp"
#include "tgbm/tools/json_tools/json_traits.hpp"
#include "tgbm/api/common.hpp"
#include "tgbm/tools/traits.hpp"

namespace tgbm::json::parse_dom {

template <discriminated_api_type T>
struct parser<T> {
  template <typename Json, json::json_traits Traits>
  static void parse(const Json& json, T& out) {
    if (!Traits::is_object(json)) {
      Traits::on_error();
    }
    constexpr std::string_view discriminator = T::discriminator;

    auto d_node = Traits::find_field(json, discriminator);
    if (!d_node) [[unlikely]] {
      Traits::on_error();
    }
    auto& d_ref = *d_node;

    if (!Traits::is_string(d_ref)) [[unlikely]] {
      Traits::on_error();
    }

    auto str = Traits::get_string(d_ref);

    T::discriminate(str, [&]<typename U>() {
      if constexpr (std::same_as<U, void>) {
        Traits::on_error();
      } else {
        parser<U>::template parse<Json, Traits>(json, out.data.template emplace<U>());
      }
    });
  }
};
}  // namespace tgbm::json::parse_dom
