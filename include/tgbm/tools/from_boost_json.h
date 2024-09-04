#pragma once

#include <boost/json.hpp>
#include "boost/type_traits/is_complete.hpp"
#include <tgbm/tools/box.hpp>
#include <tgbm/tools/pfr_extension.hpp>
#include <tgbm/tools/traits.hpp>
#include <tgbm/tools/meta.hpp>
#include <tgbm/api/common.hpp>

namespace boost::json {

struct boost_json_parse_error : std::runtime_error {
  boost_json_parse_error() : std::runtime_error("Failed parse from boost json") {
  }
};

[[noreturn]] inline void throw_boost_json_parse_error() {
  throw boost_json_parse_error();
}

#define TGBM_BOOST_JSON_FROM_ERROR throw_boost_json_parse_error()

template <tgbm::aggregate T>
T tag_invoke(const value_to_tag<T>&, const value& jv) {
  namespace pfr = boost::pfr;
  auto& object = jv.as_object();

  auto construct_field = [&]<std::size_t I>() {
    using Field = pfr::tuple_element_t<I, T>;
    auto it = object.find(boost::pfr::get_name<I, T>());
    boost::json::value null(nullptr);
    return boost::json::value_to<Field>(it != object.end() ? *it : null);
  };

  return [&]<std::size_t... I>(std::index_sequence<I...>) {
    return T{construct_field.template operator()<I>()...};
  }(std::make_index_sequence<boost::pfr::tuple_size_v<T>>{});
}

namespace details {
template <std::size_t I, typename T>
auto construct_field(const boost::json::object& object) {
  using Field = pfr::tuple_element_t<I, T>;
  auto it = object.find(boost::pfr::get_name<I, T>());
  if (it == object.end()) {
    if constexpr (T::is_optional_field(boost::pfr::get_name<I, T>())) {
      return Field{};
    } else {
      TGBM_BOOST_JSON_FROM_ERROR;
    }
  }
  return boost::json::value_to<Field>(it->value());
}
}  // namespace details

template <tgbm::common_api_type T>
T tag_invoke(const value_to_tag<T>&, const value& jv) {
  auto& object = jv.as_object();

  return [&]<std::size_t... I>(std::index_sequence<I...>) {
    return T(details::construct_field<I, T>(object)...);
  }(std::make_index_sequence<boost::pfr::tuple_size_v<T>>{});
}

inline tgbm::api::Integer tag_invoke(const value_to_tag<tgbm::api::Integer>&, const value& jv) {
  tgbm::api::Integer result;
  if (!jv.is_int64()) {
    TGBM_BOOST_JSON_FROM_ERROR;
  }
  auto data = jv.as_int64();
  if (data > result.max) {
    TGBM_BOOST_JSON_FROM_ERROR;
  }
  return data;
}

template <tgbm::discriminated_api_type T>
T tag_invoke(const value_to_tag<T>&, const value& jv) {
  constexpr std::string_view discriminator = T::discriminator;
  if (!jv.is_object()) {
    TGBM_BOOST_JSON_FROM_ERROR;
  }
  auto& object = jv.as_object();
  auto d_value = object.find(discriminator);
  if (d_value == object.end()) [[unlikely]] {
    TGBM_BOOST_JSON_FROM_ERROR;
  }
  if (!d_value->value().is_string()) [[unlikely]] {
    TGBM_BOOST_JSON_FROM_ERROR;
  }
  return T::discriminate(d_value->value().as_string(), [&]<typename U>() -> T {
    if constexpr (std::same_as<U, void>) {
      TGBM_BOOST_JSON_FROM_ERROR;
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
    TGBM_BOOST_JSON_FROM_ERROR;
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
  return std::optional<T>{std::in_place, boost::json::value_to<T>(jv)};
}

inline std::true_type tag_invoke(const value_to_tag<std::true_type>&, const value& jv) {
  if (!jv.is_bool() || !jv.as_bool()) {
    TGBM_BOOST_JSON_FROM_ERROR;
  }
  return {};
}

}  // namespace boost::json

namespace tgbm {
template <typename T>
T from_boost_json(std::string_view val) {
  using namespace boost::json;
  value j = parse(val);
  return value_to<T>(j);
}
}  // namespace tgbm
