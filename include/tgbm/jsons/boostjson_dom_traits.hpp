#pragma once

#include <boost/json.hpp>

#include <tgbm/jsons/dom_traits.hpp>
#include <tgbm/jsons/errors.hpp>
#include <tgbm/utils/macro.hpp>

namespace tgbm::json {

template <>
struct dom_traits_for<::boost::json::value> {
  using type = ::boost::json::value;

  static bool is_bool(const type& json) {
    return json.is_bool();
  }

  static bool is_integer(const type& json) {
    return json.is_int64();
  }

  static bool is_uinteger(const type& json) {
    return json.is_uint64();
  }

  static bool is_floating(const type& json) {
    return json.is_double();
  }

  static bool is_string(const type& json) {
    return json.is_string();
  }

  static bool is_object(const type& json) {
    return json.is_object();
  }

  static bool is_array(const type& json) {
    return json.is_array();
  }

  static bool is_null(const type& json) {
    return json.is_null();
  }

  static bool get_bool(const type& json) {
    return json.as_bool();
  }

  static std::int64_t get_integer(const type& json) {
    return json.as_int64();
  }

  static std::uint64_t get_uinteger(const type& json) {
    return json.as_uint64();
  }

  static double get_floating(const type& json) {
    return json.as_double();
  }

  static std::string_view get_string(const type& json) {
    return std::string_view{json.as_string().data(), json.as_string().size()};
  }

  static void on_error() {
    throw_json_parse_error();
  }

  static const type* find_field(const type& json, std::string_view key) {
    assert(json.is_object());
    auto& obj = json.as_object();
    auto it = obj.find(key);
    if (it != obj.end()) {
      return &it->value();
    }
    return nullptr;
  }

  static const type& element_by_index(const type& json, size_t index) {
    assert(json.is_array());
    auto& arr = json.as_array();
    if (index >= arr.size()) {
      on_error();
    }
    return arr[index];
  }

  static size_t size(const type& json) {
    assert(json.is_object() || json.is_array());
    if (json.is_object()) {
      return json.as_object().size();
    } else {
      return json.as_array().size();
    }
    unreachable();
  }

  static auto& member_range(const type& json) {
    if (!json.is_object()) {
      on_error();
    }
    return json.as_object();
  }
};

static_assert(dom_traits<dom_traits_for<::boost::json::value>, ::boost::json::value>);

}  // namespace tgbm::json
