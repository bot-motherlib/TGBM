#pragma once

#include <concepts>

#include "tgbm/tools/json_tools/generator_parser/basic_parser.hpp"
#include "tgbm/api/common.hpp"
#include "tgbm/tools/math.hpp"

namespace tgbm::generator_parser {

template <>
struct boost_domless_parser<bool> {
  static dd::generator<nothing_t> parse(bool& v, event_holder& tok) {
    tok.expect(event_holder::bool_);
    v = tok.bool_m;
    return {};
  }
};

template <>
struct boost_domless_parser<api::True> {
  static dd::generator<nothing_t> parse(api::True&, event_holder& tok) {
    if (tok.got != event_holder::bool_ || !tok.bool_m) [[unlikely]]
      TGBM_JSON_PARSE_ERROR;
    return {};
  }
};

template <>
struct boost_domless_parser<std::string> {
  static dd::generator<nothing_t> parse(std::string& v, event_holder& tok) {
    tok.expect(event_holder::string);
    v = tok.str_m;
    return {};
  }
};

template <>
struct boost_domless_parser<tgbm::const_string> {
  static dd::generator<nothing_t> parse(tgbm::const_string& v, event_holder& tok) {
    tok.expect(event_holder::string);
    v = tok.str_m;
    return {};
  }
};

template <typename T>
  requires(std::integral<T> || std::same_as<api::Integer, T>)
struct boost_domless_parser<T> {
  static dd::generator<nothing_t> parse(T& v, event_holder& tok) {
    using enum event_holder::wait_e;
    switch (tok.got) {
      case int64:
        safe_write(v, tok.int_m);
        return {};
      case uint64:
        safe_write(v, tok.uint_m);
        return {};
      default:
        json::throw_json_parse_error();
    }
  }
};

template <typename T>
  requires(std::floating_point<T> || std::same_as<api::Double, T>)
struct boost_domless_parser<T> {
  static dd::generator<nothing_t> parse(T& v, event_holder& tok) {
    tok.expect(event_holder::double_);
    v = tok.double_m;
    return {};
  }
};

}  // namespace tgbm::generator_parser
