#pragma once

#include <cassert>
#include <ranges>
#include <stdexcept>

#include <rapidjson/reader.h>
#include <rapidjson/document.h>
#include <rapidjson/error/en.h>

#include "tgbm/jsons/dom_traits.hpp"

namespace tgbm::json {

template <>
struct dom_traits_for<rapidjson::GenericValue<rapidjson::UTF8<>>> {
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
    return json.IsNumber();
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

  static bool get_bool(const type& json) {
    if (!json.IsBool()) {
      on_error();
    }
    return json.GetBool();
  }

  static std::int64_t get_integer(const type& json) {
    assert(json.IsInt64());
    return json.GetInt64();
  }

  static std::uint64_t get_uinteger(const type& json) {
    assert(json.IsUint64());
    return json.GetUint64();
  }

  static double get_floating(const type& json) {
    assert(json.IsNumber());
    return json.GetDouble();
  }

  static std::string_view get_string(const type& json) {
    assert(json.IsString());
    return std::string_view(json.GetString(), json.GetStringLength());
  }

  static void on_error() {
    throw std::runtime_error("JSON Error");
  }

  static const type* find_field(const type& json, std::string_view key) {
    assert(json.IsObject());
    auto it = json.FindMember(key.data());
    if (it != json.MemberEnd()) {
      return &it->value;
    }
    return nullptr;
  }

  static const type& element_by_index(const type& json, std::size_t index) {
    assert(json.IsArray());
    assert(index < json.Size());
    return json[index];
  }

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

static_assert(dom_traits<dom_traits_for<rapidjson::GenericValue<rapidjson::UTF8<>>>>);

}  // namespace tgbm::json
