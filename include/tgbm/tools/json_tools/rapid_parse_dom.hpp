#pragma once

#include <cassert>
#include <ranges>

#include <rapidjson/reader.h>
#include <rapidjson/document.h>
#include <rapidjson/error/en.h>
#include <tgbm/tools/json_tools/parse_dom/basic.hpp>

#include "tgbm/api/common.hpp"
#include "tgbm/tools/api_utils.hpp"
#include "tgbm/tools/traits.hpp"
#include "tgbm/tools/json_tools/exceptions.hpp"
#include "tgbm/tools/json_tools/json_traits.hpp"
#include "tgbm/tools/json_tools/parse_dom/basic.hpp"

namespace tgbm::json {

struct rapid_json_traits {
  using type = rapidjson::GenericValue<rapidjson::UTF8<>>;
  static bool is_bool(const type& json) {
    return json.IsBool();
  }

  static bool is_integer(const type& json) {
    return json.IsInt64() || json.IsInt();
  }

  static bool is_uinteger(const type& json) {
    return json.IsUint64() || json.IsUint();
  }

  static bool is_floating(const type& json) {
    return json.IsDouble();
  }

  static bool is_string(const type& json) {
    return json.IsString();
  }

  static bool is_object(const type& json) {
    return json.IsObject();
  }

  static bool is_array(const type& json) {
    return json.IsArray();
  }

  static bool is_null(const type& json) {
    return json.IsNull();
  }

  // Получение значений
  static bool get_bool(const type& json) {
    if (!json.IsBool()) {
      on_error();
    }
    return json.GetBool();
  }

  static std::int64_t get_integer(const type& json) {
    assert(json.IsInt64() || json.IsInt());
    if (json.IsInt64()) {
      return json.GetInt64();
    } else {
      return json.GetInt();
    }
  }

  static std::uint64_t get_uinteger(const type& json) {
    assert(json.IsUint() || json.IsUint64());
    if (json.IsUint64()) {
      return json.GetInt64();
    } else {
      return json.GetInt();
    }
  }

  static double get_floating(const type& json) {
    assert(json.IsDouble());
    return json.GetDouble();
  }

  static std::string_view get_string(const type& json) {
    assert(json.IsString());
    return std::string_view(json.GetString(), json.GetStringLength());
  }

  // Обработка ошибок
  static void on_error() {
    throw std::runtime_error("JSON Error");
  }

  // Поиск поля в объекте
  static const type* find_field(const type& json, std::string_view key) {
    assert(json.IsObject());
    auto it = json.FindMember(key.data());
    if (it != json.MemberEnd()) {
      return &it->value;
    }
    return nullptr;
  }

  // Доступ к элементу по индексу
  static const type& element_by_index(const type& json, std::size_t index) {
    assert(json.IsArray());
    assert(index < json.Size());
    return json[index];
  }

  // Размер объекта или массива
  static std::size_t size(const type& json) {
    assert(json.IsObject() || json.IsArray());
    if (json.IsObject()) {
      return json.MemberCount();
    } else {
      return json.Size();
    }
  }
  struct key_value {
    std::string_view name;
    const type& value;
  };

  // Диапазон членов объекта
  static auto member_range(const type& json) {
    if (!json.IsObject()) {
      on_error();
    }
    auto transformer = [](type::ConstMemberIterator it) {
      return key_value{.name = {it->name.GetString(), it->name.GetStringLength()}, .value = it->value};
    };
    return std::ranges::views::iota(json.MemberBegin(), json.MemberEnd()) |
           std::ranges::views::transform(transformer);
  }
};

static_assert(tgbm::json::json_traits<rapid_json_traits>);

}  // namespace tgbm::json

namespace tgbm::json::rapid {

template <typename T>
inline T parse_dom(std::string_view sv) {
  T t;
  rapidjson::Document document;
  document.Parse(sv.data(), sv.size());
  if (document.HasParseError()) {
    TGBM_JSON_PARSE_ERROR;
  }
  tgbm::json::parse_dom::parser<T>::template parse<rapid_json_traits::type, rapid_json_traits>(document, t);
  return t;
}

}  // namespace tgbm::json::rapid
