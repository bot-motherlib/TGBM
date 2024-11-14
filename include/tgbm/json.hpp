#pragma once

#include "tgbm/jsons/stream_parser.hpp"
#include "tgbm/jsons/json_traits.hpp"
#include "tgbm/jsons/parse_dom/basic.hpp"

namespace tgbm {

template <typename T, json::json_dom Json>
void from_json(const Json& j, T& out) {
  json::parse_dom::parser<T>::template parse<Json, json::default_traits<Json>>(j, out);
}

template <typename T>
[[nodiscard]] T from_json(const json::json_dom auto& j) {
  T out;
  from_json(j, out);
  return out;
}

template <typename T>
void from_json(std::string_view json, T& out) {
  json::stream_parser p(out);
  p.feed(json, true);
}

template <typename T>
[[nodiscard]] T from_json(std::string_view json) {
  T out;
  from_json(json, out);
  return out;
}

}  // namespace tgbm
