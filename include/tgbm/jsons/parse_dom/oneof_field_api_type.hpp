#pragma once

#include <tgbm/jsons/parse_dom/basic.hpp>
#include <tgbm/jsons/dom_traits.hpp>
#include <tgbm/api/common.hpp>
#include <tgbm/utils/api_utils.hpp>
#include <tgbm/utils/traits.hpp>

namespace tgbm::json::parse_dom {

template <oneof_field_api_type T>
struct parser<T> {
  template <typename Json, json::dom_traits Traits>
  static void parse(const Json& json, T& out) {
    if (!Traits::is_object(json)) [[unlikely]] {
      Traits::on_error();
    }
    constexpr size_t N = tgbm::oneof_field_utils::N<T>;

    using Data = decltype(T::data);

    // clang-format off
    auto m = matcher{
      [&]<typename Info, typename Field>(Field& req_field) {
        constexpr std::string_view name = Info::name.AsStringView();
        auto node = Traits::find_field(json, name);
        if (!node || Traits::is_null(*node)){
          Traits::on_error();
        }
        parser<Field>::template parse<Json, Traits>(*node, req_field);
      }, 
      [&]<typename Info>(Data& data){
        for (auto&& [key, value] : Traits::member_range(json)) {
          if (!tgbm::oneof_field_utils::is_required<T>(key)) {
            tgbm::oneof_field_utils::emplace_field<T, void>(
                data, key,
                [&]<typename Field>(Field& field) { parser<Field>::template parse<Json, Traits>(value, field); },
                [] { Traits::on_error(); }, [] {});
            return;
          }
        }
      }
    };
    // clang-format on

    pfr_extension::visit_object(out, m);
  }
};

}  // namespace tgbm::json::parse_dom
