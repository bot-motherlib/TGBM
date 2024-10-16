#pragma once
<<<<<<< HEAD

#include <concepts>
#include <string>

#include "tgbm/tools/json_tools/parse_dom/basic.hpp"
#include "tgbm/tools/json_tools/json_traits.hpp"
#include "tgbm/api/common.hpp"
=======
#include <concepts>
#include <tgbm/tools/json_tools/parse_dom/basic.hpp>
#include <tgbm/tools/json_tools/json_traits.hpp>
#include <tgbm/api/common.hpp>
#include <string>
>>>>>>> e14721b (first impl)
#include "tgbm/tools/math.hpp"

namespace tgbm::json::parse_dom {

template <>
struct parser<std::string> {
  template <typename Json, json::json_traits Traits = json::default_json_traits<Json>>
  static void parse(const Json& json, std::string& out) {
    if (Traits::is_string(json)) {
      out = Traits::get_string(json);
    } else {
      Traits::on_error();
    }
  }
};

template <>
struct parser<tgbm::const_string> {
  template <typename Json, json::json_traits Traits = json::default_json_traits<Json>>
  static void parse(const Json& json, tgbm::const_string& out) {
    if (Traits::is_string(json)) {
      out = Traits::get_string(json);
    } else {
      Traits::on_error();
    }
  }
};

template <std::integral T>
struct parser<T> {
  template <typename Json, json::json_traits Traits = json::default_json_traits<Json>>
  static void parse(const Json& json, T& out) {
    if (Traits::is_uinteger(json)) {
      safe_write(out, Traits::get_uinteger(json));
    } else if (Traits::is_integer(json)) {
      safe_write(out, Traits::get_integer(json));
    } else {
      Traits::on_error();
    }
  }
};

template <std::floating_point T>
struct parser<T> {
  template <typename Json, json::json_traits Traits>
  static void parse(const Json& json, T& out) {
    if (Traits::is_floating(json)) {
      out = Traits::get_floating(json);
    } else {
      Traits::on_error();
    }
  }
};

template <>
struct parser<tgbm::api::Integer> {
  template <typename Json, json::json_traits Traits>
  static void parse(const Json& json, tgbm::api::Integer& out) {
    if (Traits::is_integer(json)) {
      safe_write(out, Traits::get_integer(json));
    } else if (Traits::is_uinteger(json)) {
      safe_write(out, Traits::get_uinteger(json));
    } else {
      Traits::on_error();
    }
  }
};

template <>
struct parser<bool> {
  template <typename Json, json::json_traits Traits>
  static void parse(const Json& json, bool& out) {
    if (Traits::is_bool(json)) {
      out = Traits::get_bool(json);
    } else {
      Traits::on_error();
    }
  }
};

template <>
struct parser<tgbm::api::True> {
  template <typename Json, json::json_traits Traits>
  static void parse(const Json& json, tgbm::api::True& out) {
<<<<<<< HEAD
    if (!Traits::is_bool(json) || !Traits::get_bool(json)) {
=======
    if (!Traits::is_bool(json) || Traits::get_bool(json)) {
>>>>>>> e14721b (first impl)
      Traits::on_error();
    }
  }
};

template <>
struct parser<api::Double> {
  template <typename Json, json::json_traits Traits>
  static void parse(const Json& json, api::Double& out) {
    if (Traits::is_floating(json)) {
      out = Traits::get_floating(json);
    } else {
      Traits::on_error();
    }
  }
};

}  // namespace tgbm::json::parse_dom
