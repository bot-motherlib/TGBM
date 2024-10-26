#pragma once

#include <boost/json.hpp>
#include <boost/json/visit.hpp>
#include <boost/container/string.hpp>

#include "tgbm/tools/box.hpp"
#include "tgbm/tools/pfr_extension.hpp"
#include "tgbm/tools/traits.hpp"
#include "tgbm/tools/meta.hpp"
#include "tgbm/api/common.hpp"
#include "tgbm/tools/api_utils.hpp"
#include "tgbm/tools/json_tools/exceptions.hpp"
#include "tgbm/tools/json_tools/json_traits.hpp"
#include "tgbm/tools/json_tools/parse_dom/basic.hpp"

namespace tgbm::json {
struct boost_json_traits {
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

  // Получение значений
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

  // Обработка ошибок
  static void on_error() {
    throw std::runtime_error("JSON Error");
  }

  // Поиск поля в объекте
  static const type* find_field(const type& json, std::string_view key) {
    assert(json.is_object());
    auto& obj = json.as_object();
    auto it = obj.find(key);
    if (it != obj.end()) {
      return &it->value();
    }
    return nullptr;
  }

  // Доступ к элементу по индексу
  static const type& element_by_index(const type& json, std::size_t index) {
    assert(json.is_array());
    auto& arr = json.as_array();
    if (index >= arr.size()) {
      on_error();
    }
    return arr[index];
  }

  // Размер объекта или массива
  static std::size_t size(const type& json) {
    assert(json.is_object() || json.is_array());
    if (json.is_object()) {
      return json.as_object().size();
    } else {
      return json.as_array().size();
    }
    unreachable();
  }

  // Диапазон членов объекта
  static auto& member_range(const type& json) {
    if (!json.is_object()) {
      on_error();
    }

    return json.as_object();
  }
};

static_assert(json_traits<boost_json_traits, ::boost::json::value>);
}  // namespace tgbm::json

namespace tgbm::json::boost {

template <typename T>
T parse_dom(std::string_view val) {
  using namespace ::boost::json;
  value j = parse(val);
  T out;
  parse_dom::parser<T>::template parse<::boost::json::value, boost_json_traits>(j, out);
  return out;
}

}  // namespace tgbm::json::boost
