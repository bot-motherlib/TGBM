#pragma once
#include <tgbm/tools/json_tools/parse_dom/basic.hpp>
#include <tgbm/tools/json_tools/json_traits.hpp>
#include <tgbm/api/common.hpp>
#include "tgbm/tools/pfr_extension.hpp"
#include "tgbm/tools/traits.hpp"

namespace tgbm::json::parse_dom {

template <common_api_type T>
struct parser<T> {
  template <typename Json, json::json_traits Traits>
  static void parse(const Json& json, T& out) {
    if (!Traits::is_object(json)) {
      Traits::on_error();
    }
    pfr_extension::visit_object(out, [&]<typename Info, typename Field>(Field& field) {
      constexpr std::string_view name = Info::name.AsStringView();
      auto json_field = Traits::find_field(json, name);
      if (!json_field) [[unlikely]] {
        if constexpr (T::is_optional_field(name)) {
          field = Field{};
          return;
        } else {
          Traits::on_error();
        }
      }
      parser<Field>::template parse<Json, Traits>(*json_field, field);
    });
  }
};
}  // namespace tgbm::json::parse_dom
