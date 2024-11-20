#pragma once

#include <tgbm/jsons/stream_parser.hpp>
#include <tgbm/jsons/dom_traits.hpp>
#include <tgbm/jsons/boostjson_dom_traits.hpp>
#include <tgbm/jsons/rapidjson_serialize_sax.hpp>
#include <tgbm/jsons/boostjson_serialize_dom.hpp>

#include <tgbm/jsons/parse_dom/basic.hpp>

namespace tgbm {

template <typename T, json::dom Json>
void from_json(const Json& j, T& out) {
  json::parse_dom::parser<T>::template parse<Json, json::dom_traits_for<Json>>(j, out);
}

template <typename T>
[[nodiscard]] T from_json(const json::dom auto& j) {
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

// TODO more overloads with output iterator/buffer + good bytes_t (for rapidjson PutUnsafe PutReserve etc)
template <typename T>
void to_json(const T& t, bytes_t& out) {
  rj_refbuffer_t buf(out);
  rapidjson::Writer w(buf);
  rj_tojson<decltype(w), T>::serialize(w, t);
}

template <typename T>
void to_json(const T& v, boost::json::value& out) {
  boost::json::value_from(v, out);
}

}  // namespace tgbm
