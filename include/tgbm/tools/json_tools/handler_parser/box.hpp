#pragma once

#include <tgbm/tools/box.hpp>
#include <tgbm/tools/json_tools/handler_parser/basic_parser.hpp>
#include <cassert>
#include <optional>

namespace tgbm::json::handler_parser {

template <typename T>
struct parser<box<T>> : basic_parser<box<T>> {
  using basic_parser<box<T>>::basic_parser;

  parser<T> emplace() {
    this->t_->emplace();
    return parser<T>(this->parser_stack_, **this->t_);
  }

  ResultParse on_next_end_parsing() {
    assert(this->t_);
    return ResultParse::kEnd;
  }

  ResultParse handle_result(auto&& callable) {
    this->parser_stack_.push(emplace());
    return callable(this->parser_stack_.last());
  }

  ResultParse start_object() {
    return handle_result([](auto& parser) { return parser.start_object(); });
  }
  ResultParse start_array() {
    return handle_result([](auto& parser) { return parser.start_array(); });
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
    return ResultParse::kEnd;
  }
};

}  // namespace tgbm::json::handler_parser
