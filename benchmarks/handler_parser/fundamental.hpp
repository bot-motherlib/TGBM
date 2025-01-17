#pragma once

#include "basic_parser.hpp"
#include <tgbm/api/Integer.hpp>
#include <tgbm/api/floating.hpp>
#include <tgbm/api/common.hpp>

namespace tgbm::json::handler_parser {

template <std::floating_point T>
struct parser<T> : basic_parser<T> {
  using basic_parser<T>::basic_parser;

  ResultParse floating(double val) {
    *this->t_ = val;
    return ResultParse::kEnd;
  }
};

template <std::integral T>
struct parser<T> : basic_parser<T> {
  using basic_parser<T>::basic_parser;

  ResultParse integral(std::int64_t val) {
    *this->t_ = val;
    return ResultParse::kEnd;
  }
  ResultParse unsigned_integral(std::uint64_t val) {
    *this->t_ = val;
    return ResultParse::kEnd;
  }
};

template <>
struct parser<api::Double> : basic_parser<api::Double> {
  using basic_parser<api::Double>::basic_parser;

  ResultParse floating(double val) {
    *this->t_ = val;
    return ResultParse::kEnd;
  }
};

template <>
struct parser<tgbm::const_string> : basic_parser<tgbm::const_string> {
  using basic_parser<tgbm::const_string>::basic_parser;

  ResultParse string(std::string_view val) {
    *this->t_ = val;
    return ResultParse::kEnd;
  }
};

template <>
struct parser<api::Integer> : basic_parser<api::Integer> {
  using basic_parser<api::Integer>::basic_parser;

  ResultParse integral(std::int64_t val) {
    if (val > api::Integer::max) {
      TGBM_JSON_HANDLER_PARSE_ERROR;
    }
    *this->t_ = val;
    return ResultParse::kEnd;
  }
  ResultParse unsigned_integral(std::uint64_t val) {
    if (val > std::numeric_limits<std::int64_t>::max()) {
      TGBM_JSON_HANDLER_PARSE_ERROR;
    }
    if (val > api::Integer::max) {
      TGBM_JSON_HANDLER_PARSE_ERROR;
    }
    *this->t_ = val;
    return ResultParse::kEnd;
  }
};

template <>
struct parser<bool> : basic_parser<bool> {
  using basic_parser<bool>::basic_parser;

  ResultParse boolean(bool val) {
    *this->t_ = val;
    return ResultParse::kEnd;
  }
};

template <>
struct parser<api::True> : basic_parser<api::True> {
  using basic_parser<api::True>::basic_parser;

  ResultParse boolean(bool val) {
    if (val) {
      return ResultParse::kEnd;
    } else {
      TGBM_JSON_HANDLER_PARSE_ERROR;
    }
  }
};
}  // namespace tgbm::json::handler_parser
