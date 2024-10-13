#pragma once
#include <concepts>
#include <tgbm/tools/json_tools/generator_parser/basic_parser.hpp>
#include <tgbm/api/common.hpp>
#include "tgbm/tools/math.hpp"

namespace tgbm::generator_parser {
template <>
struct boost_domless_parser<bool> {
  static constexpr bool simple = true;
  static void simple_parse(bool& t_, event_holder& holder) {
    holder.expect(event_holder::bool_);
    t_ = holder.bool_m;
  }
};

template <>
struct boost_domless_parser<api::True> {
  static constexpr bool simple = true;
  static void simple_parse(api::True& t_, event_holder& holder) {
    holder.expect(event_holder::bool_);
    if (!holder.bool_m) {
      TGBM_JSON_PARSE_ERROR;
    }
  }
};

template <>
struct boost_domless_parser<std::string> {
  static constexpr bool simple = true;
  static void simple_parse(std::string& t_, event_holder& holder) {
    holder.expect(event_holder::string);
    t_ = holder.str_m;
  }
};

template <std::integral T>
struct boost_domless_parser<T> {
  static constexpr bool simple = true;
  static void simple_parse(T& t_, event_holder& holder) {
    holder.expect(event_holder::int64 | event_holder::uint64);
    if (holder.got == event_holder::uint64) {
      safe_write(t_, holder.uint_m);
    } else {
      safe_write(t_, holder.int_m);
    }
  }
};

template <>
struct boost_domless_parser<api::Integer> {
  static constexpr bool simple = true;
  static void simple_parse(api::Integer& t_, event_holder& holder) {
    holder.expect(event_holder::int64 | event_holder::uint64);
    if (holder.got == event_holder::uint64) {
      safe_write(t_, holder.uint_m);
    } else {
      safe_write(t_, holder.int_m);
    }
  }
};

template <>
struct boost_domless_parser<api::Double> {
  static constexpr bool simple = true;
  static void simple_parse(api::Double& t_, event_holder& holder) {
    holder.expect(event_holder::double_);
    t_ = holder.double_m;
  }
};

template <>
struct boost_domless_parser<double> {
  static constexpr bool simple = true;
  static void simple_parse(double& t_, event_holder& holder) {
    holder.expect(event_holder::double_);
    t_ = holder.double_m;
  }
};

template <>
struct boost_domless_parser<float> {
  static constexpr bool simple = true;
  static void simple_parse(float& t_, event_holder& holder) {
    holder.expect(event_holder::double_);
    t_ = holder.double_m;
  }
};
}  // namespace tgbm::generator_parser
