#pragma once

#include "tgbm/jsons/parse_dom/basic.hpp"
#include "tgbm/jsons/dom_traits.hpp"
#include "tgbm/api/common.hpp"

namespace tgbm::json::parse_dom {

template <typename Optional>
struct basic_optional_parser {
  using T = typename Optional::value_type;
  template <typename Json, json::dom_traits Traits>
  static void parse(const Json& j, Optional& out) {
    if (Traits::is_null(j)) {
      return;
    } else {
      parser<T>::template parse<Json, Traits>(j, out.emplace());
    }
  }
};

template <typename T>
struct parser<box<T>> : basic_optional_parser<box<T>> {};

template <typename T>
struct parser<tgbm::api::optional<T>> : basic_optional_parser<tgbm::api::optional<T>> {};

template <typename T>
struct parser<std::optional<T>> : basic_optional_parser<std::optional<T>> {};

template <>
struct parser<nothing_t> {
  template <typename Json, json::dom_traits Traits>
  static void parse(const Json& j, nothing_t& out) {
    if (!Traits::is_null(j)) {
      Traits::on_error();
    }
  }
};

}  // namespace tgbm::json::parse_dom
