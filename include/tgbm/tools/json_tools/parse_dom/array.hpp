#pragma once

#include <vector>

#include "tgbm/tools/json_tools/parse_dom/basic.hpp"
#include "tgbm/tools/json_tools/json_traits.hpp"

namespace tgbm::json::parse_dom {

template <typename T>
struct parser<std::vector<T>> {
  template <typename Json, json::json_traits Traits>
  static void parse(const Json& json, std::vector<T>& out) {
    if (!Traits::is_array(json)) {
      Traits::on_error();
    }
    std::size_t size = Traits::size(json);
    out.reserve(size);
    for (std::size_t i = 0; i < size; i++) {
      auto& element_node = Traits::element_by_index(json, i);
      parser<T>::template parse<Json, Traits>(element_node, out.emplace_back());
    }
  }
};
}  // namespace tgbm::json::parse_dom
