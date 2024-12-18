#pragma once

#include <vector>

#include <tgbm/utils/traits.hpp>
#include "basic_parser.hpp"

namespace tgbm::json::handler_parser {

template <string_like T>
struct parser<T> : basic_parser<T> {
  using basic_parser<T>::basic_parser;

  ResultParse string(std::string_view val) {
    *this->t_ = T{val};
    return ResultParse::kEnd;
  }
};

template <>
struct parser<std::string_view> {};

template <typename T>
struct parser<std::vector<T>> : basic_parser<std::vector<T>> {
  using basic_parser<std::vector<T>>::basic_parser;
  bool started_ = false;

  parser<T> emplace() {
    auto& elem = this->t_->emplace_back();
    return parser<T>(this->parser_stack_, elem);
  }

  ResultParse handle_result(auto&& callable) {
    if (!started_) {
      return ResultParse::kError;
    }
    this->parser_stack_.push(emplace());
    return callable(this->parser_stack_.last());
  }

  ResultParse start_array() {
    using Parser = parser<T>;
    if (started_) {
      return handle_result([](auto& parser) { return parser.start_array(); });
    } else {
      started_ = true;
      return ResultParse::kContinue;
    }
  }

  ResultParse end_array() {
    return ResultParse::kEnd;
  }

  ResultParse start_object() {
    return handle_result([](auto& parser) { return parser.start_object(); });
  }

  ResultParse key(std::string_view val) {
    return handle_result([val](auto& parser) { return parser.key(val); });
  }
  ResultParse string(std::string_view val) {
    return handle_result([val](auto& parser) { return parser.string(val); });
  }
  ResultParse integral(std::int64_t val) {
    return handle_result([val](auto& parser) { return parser.integral(val); });
  }
  ResultParse unsigned_integral(std::uint64_t val) {
    return handle_result([val](auto& parser) { return parser.unsigned_integral(val); });
  }
  ResultParse floating(double val) {
    return handle_result([val](auto& parser) { return parser.floating(val); });
  }
  ResultParse boolean(bool val) {
    return handle_result([val](auto& parser) { return parser.boolean(val); });
  }

  ResultParse null() {
    return handle_result([](auto& parser) { return parser.null(); });
  }
};

}  // namespace tgbm::json::handler_parser
