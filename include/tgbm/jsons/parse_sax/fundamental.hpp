#pragma once

#include <concepts>

#include <kelcoro/stack_memory_resource.hpp>

#include <tgbm/jsons/sax.hpp>
#include <tgbm/api/common.hpp>
#include <tgbm/utils/math.hpp>

namespace tgbm::json {

template <>
struct sax_parser<bool> {
  static sax_consumer_t parse(bool& v, sax_token& tok, dd::with_stack_resource) {
    tok.expect(sax_token::bool_);
    v = tok.bool_m;
    return {};
  }
};

template <>
struct sax_parser<api::True> {
  static sax_consumer_t parse(api::True&, sax_token& tok, dd::with_stack_resource r) {
    if (tok.got != sax_token::bool_ || !tok.bool_m) [[unlikely]]
      TGBM_JSON_PARSE_ERROR;
    return {};
  }
};

template <>
struct sax_parser<std::string> {
  static sax_consumer_t parse(std::string& v, sax_token& tok, dd::with_stack_resource r) {
    tok.expect(sax_token::string);
    v = tok.str_m;
    return {};
  }
};

template <>
struct sax_parser<tgbm::const_string> {
  static sax_consumer_t parse(tgbm::const_string& v, sax_token& tok, dd::with_stack_resource r) {
    tok.expect(sax_token::string);
    v = tok.str_m;
    return {};
  }
};

template <typename T>
  requires(std::integral<T> || std::same_as<api::Integer, T>)
struct sax_parser<T> {
  static sax_consumer_t parse(T& v, sax_token& tok, dd::with_stack_resource r) {
    using enum sax_token::kind_e;
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
struct sax_parser<T> {
  static sax_consumer_t parse(T& v, sax_token& tok, dd::with_stack_resource r) {
    tok.expect(sax_token::double_);
    v = tok.double_m;
    return {};
  }
};

}  // namespace tgbm::json
