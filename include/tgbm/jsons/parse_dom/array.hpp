#pragma once

#include <vector>

#include <tgbm/jsons/parse_dom/basic.hpp>
#include <tgbm/jsons/dom_traits.hpp>

namespace tgbm::json::parse_dom {

template <typename T>
struct parser<std::vector<T>> {
  template <typename Json, json::dom_traits Traits>
  static void parse(const Json& json, std::vector<T>& out) {
    if (!Traits::is_array(json)) {
      Traits::on_error();
    }
    size_t size = Traits::size(json);
    out.reserve(size);
    for (size_t i = 0; i < size; i++) {
      auto& element_node = Traits::element_by_index(json, i);
      parser<T>::template parse<Json, Traits>(element_node, out.emplace_back());
    }
  }
};

}  // namespace tgbm::json::parse_dom
