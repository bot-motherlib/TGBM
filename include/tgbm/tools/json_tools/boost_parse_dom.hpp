#pragma once

#include <boost/json.hpp>
#include "boost/pfr/core_name.hpp"
#include "boost/pfr/tuple_size.hpp"
#include <tgbm/tools/box.hpp>
#include <tgbm/tools/pfr_extension.hpp>
#include <tgbm/tools/traits.hpp>
#include <tgbm/tools/meta.hpp>
#include <tgbm/api/common.hpp>
#include <boost/json/visit.hpp>
#include <boost/container/string.hpp>
#include <tgbm/tools/api_utils.hpp>
#include <tgbm/tools/json_tools/exceptions.hpp>

namespace boost::json {

template <tgbm::aggregate T>
T tag_invoke(const value_to_tag<T>&, const value& jv) {
  if (!jv.is_object()) {
    TGBM_JSON_PARSE_ERROR;
  }
  auto& object = jv.as_object();

  auto construct_field = [&]<std::size_t I>() {
    using Field = pfr::tuple_element_t<I, T>;
    auto it = object.find(pfr_extension::get_name<I, T>());
    boost::json::value null(nullptr);
    return boost::json::value_to<Field>(it != object.end() ? *it : null);
  };

  return [&]<std::size_t... I>(std::index_sequence<I...>) {
    return T{construct_field.template operator()<I>()...};
  }(std::make_index_sequence<pfr_extension::tuple_size_v<T>>{});
}

namespace details {

template <std::size_t I, typename T>
auto construct_field(const boost::json::object& object) {
  using Field = pfr::tuple_element_t<I, T>;
  constexpr std::string_view name = pfr_extension::get_name<I, T>();
  auto it = object.find(name);
  if (it == object.end()) {
    if constexpr (T::is_optional_field(name)) {
      return Field{};
    } else {
      TGBM_JSON_PARSE_ERROR;
    }
  }
  return boost::json::value_to<Field>(it->value());
}

template <std::size_t I, typename T>
auto construct_req_field(const boost::json::object& object) {
  using Field = pfr::tuple_element_t<I, T>;
  constexpr std::string_view name = pfr_extension::element_name_v<I, T>;
  auto it = object.find(name);
  if (it == object.end()) {
    TGBM_JSON_PARSE_ERROR;
  }
  return boost::json::value_to<Field>(it->value());
}

}  // namespace details

template <tgbm::common_api_type T>
T tag_invoke(const value_to_tag<T>&, const value& jv) {
  if (!jv.is_object()) {
    TGBM_JSON_PARSE_ERROR;
  }
  auto& object = jv.as_object();

  return [&]<std::size_t... I>(std::index_sequence<I...>) {
    return T(details::construct_field<I, T>(object)...);
  }(std::make_index_sequence<pfr_extension::tuple_size_v<T>>{});
}

inline tgbm::api::Integer tag_invoke(const value_to_tag<tgbm::api::Integer>&, const value& jv) {
  tgbm::api::Integer result;
  if (!jv.is_int64()) {
    TGBM_JSON_PARSE_ERROR;
  }
  auto data = jv.as_int64();
  if (data > result.max) {
    TGBM_JSON_PARSE_ERROR;
  }
  return data;
}

template <tgbm::discriminated_api_type T>
T tag_invoke(const value_to_tag<T>&, const value& jv) {
  constexpr std::string_view discriminator = T::discriminator;
  if (!jv.is_object()) {
    TGBM_JSON_PARSE_ERROR;
  }
  auto& object = jv.as_object();
  auto d_value = object.find(discriminator);
  if (d_value == object.end()) [[unlikely]] {
    TGBM_JSON_PARSE_ERROR;
  }
  if (!d_value->value().is_string()) [[unlikely]] {
    TGBM_JSON_PARSE_ERROR;
  }
  return T::discriminate(d_value->value().as_string(), [&]<typename U>() -> T {
    if constexpr (std::same_as<U, void>) {
      TGBM_JSON_PARSE_ERROR;
    } else {
      return T{boost::json::value_to<U>(jv)};
    }
  });
}

template <typename T>
tgbm::box<T> tag_invoke(const value_to_tag<tgbm::box<T>>&, const value& jv) {
  if (jv.is_null()) [[unlikely]] {
    return nullptr;
  }
  return tgbm::box<T>{std::in_place, boost::json::value_to<T>(jv)};
}

template <typename T>
std::vector<T> tag_invoke(const value_to_tag<std::vector<T>>&, const value& jv) {
  if (!jv.is_array()) {
    TGBM_JSON_PARSE_ERROR;
  }
  auto ja = jv.as_array();
  std::vector<T> result;
  result.reserve(ja.size());
  for (auto& elem : ja) {
    result.emplace_back(boost::json::value_to<T>(elem));
  }
  return result;
}

template <typename T>
std::optional<T> tag_invoke(const value_to_tag<std::optional<T>>&, const value& jv) {
  if (jv.is_null()) [[unlikely]] {
    return std::nullopt;
  }
  return std::optional<T>{boost::json::value_to<T>(jv)};
}

template <typename T>
tgbm::api::optional<T> tag_invoke(const value_to_tag<tgbm::api::optional<T>>&, const value& jv) {
  if (jv.is_null()) [[unlikely]] {
    return std::nullopt;
  }
  return tgbm::api::optional<T>{boost::json::value_to<T>(jv)};
}

inline tgbm::api::Double tag_invoke(const value_to_tag<tgbm::api::Double>&, const value& jv) {
  if (!jv.is_double()) {
    TGBM_JSON_PARSE_ERROR;
  }
  return jv.as_double();
}

inline tgbm::api::True tag_invoke(const value_to_tag<tgbm::api::True>&, const value& jv) {
  if (!jv.is_bool() || !jv.as_bool()) {
    TGBM_JSON_PARSE_ERROR;
  }
  return {};
}

inline tgbm::nothing_t tag_invoke(const value_to_tag<tgbm::nothing_t>&, const value& jv) {
  if (!jv.is_object()) {
    TGBM_JSON_PARSE_ERROR;
  }
  return {};
}

template <tgbm::oneof_field_api_type T>
T tag_invoke(const value_to_tag<T>&, const value& jv) {
  if (!jv.is_object()) {
    TGBM_JSON_PARSE_ERROR;
  }
  auto& jv_obj = jv.as_object();
  constexpr std::size_t N = tgbm::oneof_field_utils::N<T>;

  using Data = decltype(T::data);
  auto construct_data_field = [&]() {
    Data data;
    for (auto&& [key, value] : jv_obj) {
      if (!tgbm::oneof_field_utils::is_required<T>(key)) {
        tgbm::oneof_field_utils::emplace_field<T, void>(
            data, key, [&]<typename Field>(Field& field) { field = boost::json::value_to<Field>(value); },
            [] { TGBM_JSON_PARSE_ERROR; }, [] {});
      }
    }
    return data;
  };

  return tgbm::call_on_indexes<N>([&]<std::size_t... I> {
    return T{details::construct_req_field<I, T>(jv_obj)..., construct_data_field()};
  });
}

}  // namespace boost::json

namespace tgbm::json::boost {

template <typename T>
T parse_dom(std::string_view val) {
  using namespace ::boost::json;
  value j = parse(val);
  return value_to<T>(j);
}

}  // namespace tgbm::json::boost
