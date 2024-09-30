#pragma once

#include <tgbm/tools/json_tools/handler_parser/basic_parser.hpp>

namespace tgbm::json::handler_parser {

struct ignore_parser : basic_parser<void> {
  std::uint64_t count_ = 0;

  using basic_parser<void>::basic_parser;

  ResultParse on_next_end_parsing() {
    TGBM_JSON_HANDLER_PARSE_ERROR;
  }

  ResultParse start_object() {
    count_++;
    return ResultParse::kContinue;
  }
  ResultParse end_object() {
    count_--;
    if (count_ == 0) {
      return ResultParse::kEnd;
    } else {
      return ResultParse::kContinue;
    }
  }
  ResultParse start_array() {
    count_++;
    return ResultParse::kContinue;
  }
  ResultParse end_array() {
    count_--;
    if (count_ == 0) {
      return ResultParse::kEnd;
    } else {
      return ResultParse::kContinue;
    }
  }
  ResultParse key(std::string_view val) {
    return ResultParse::kContinue;
  }
  ResultParse string(std::string_view val) {
    return ResultParse::kContinue;
  }
  ResultParse integral(std::int64_t val) {
    return ResultParse::kContinue;
  }
  ResultParse unsigned_integral(std::uint64_t val) {
    return ResultParse::kContinue;
  }
  ResultParse floating(double val) {
    return ResultParse::kContinue;
  }
  ResultParse boolean(bool val) {
    return ResultParse::kContinue;
  }
  ResultParse null() {
    return ResultParse::kContinue;
  }
};

}  // namespace tgbm::json::handler_parser
