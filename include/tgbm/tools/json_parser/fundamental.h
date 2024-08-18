#pragma once

#if 0

#include <tgbm/tools/json_parser/basic.h>

namespace tgbm::json {
template <std::floating_point T>
struct parser<T> : basic_parser<T> {
  parser(T& t) : basic_parser<T>(t) {
  }
  parser(T* t) : basic_parser<T>(t) {
  }

  ResultParse floating(double val) {
    assert(!this->parsed_);
    this->parsed_ = true;
    *this->t_ = val;
    return ResultParse::kEnd;
  }
};

template <std::integral T>
struct parser<T> : basic_parser<T> {
  parser(T& t) : basic_parser<T>(t) {
  }
  parser(T* t) : basic_parser<T>(t) {
  }

  ResultParse integral(std::int64_t val) {
    assert(!this->parsed_);
    *this->t_ = val;
    this->parsed_ = true;
    return ResultParse::kEnd;
  }
  ResultParse unsigned_integral(std::uint64_t val) {
    assert(!this->parsed_);
    *this->t_ = val;
    this->parsed_ = true;
    return ResultParse::kEnd;
  }
};

template <>
struct parser<bool> : basic_parser<bool> {
  parser(bool& t) : basic_parser<bool>(t) {
  }
  parser(bool* t) : basic_parser<bool>(t) {
  }

  ResultParse boolean(bool val) {
    assert(!parsed_);
    parsed_ = true;
    *this->t_ = val;
    return ResultParse::kEnd;
  }
};

template <>
struct parser<std::true_type> : basic_parser<std::true_type> {
  parser(std::true_type& t) : basic_parser<std::true_type>(t) {
  }
  parser(std::true_type* t) : basic_parser<std::true_type>(t) {
  }

  ResultParse boolean(bool val) {
    if (val) {
      return ResultParse::kEnd;
    } else {
      return ResultParse::kError;
    }
  }
};
}  // namespace tgbm::json

#endif
