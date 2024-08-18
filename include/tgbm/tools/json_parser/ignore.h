#pragma once

#if 0
#include <tgbm/tools/json_parser/basic.h>

namespace tgbm::json {

struct ignore_parser {
  enum type : char { kObject, kArray, kKey };
  void push_back(type t) {
    path_[size_++] = t;
  }
  void pop_back() {
    assert(size_ > 0);
    size_--;
  }
  bool empty() {
    return size_ == 0;
  }
  type back() {
    assert(size_ > 0);
    return path_[size_ - 1];
  }
  void on_start_parsing() {
  }
  ResultParse start_object() {
    if (parsed_) {
      JSON_PARSE_ERROR;
    }
    push_back(kObject);
    return ResultParse::kContinue;
  }
  ResultParse end_object() {
    if (empty() || parsed_) {
      JSON_PARSE_ERROR;
    }
    auto last = back();
    if (last == kObject) {
      pop_back();
      if (empty()) {
        return ResultParse::kEnd;
      } else
        return ResultParse::kContinue;
    }
    JSON_PARSE_ERROR;
  }
  ResultParse start_array() {
    if (parsed_) {
      JSON_PARSE_ERROR;
    }
    push_back(kArray);
    return ResultParse::kContinue;
  }
  ResultParse end_array() {
    if (parsed_) {
      JSON_PARSE_ERROR;
    }
    if (empty()) {
      JSON_PARSE_ERROR;
    }
    auto last = back();
    if (last == kArray) {
      pop_back();
      if (empty()) {
        return ResultParse::kEnd;
      } else
        return ResultParse::kContinue;
    }
    JSON_PARSE_ERROR;
  }
  ResultParse key(std::string_view val) {
    if (parsed_ || empty()) {
      JSON_PARSE_ERROR;
    }
    if (back() != kObject) {
      JSON_PARSE_ERROR;
    }
    push_back(kKey);
    return ResultParse::kContinue;
  }
  ResultParse value() {
    if (parsed_) {
      JSON_PARSE_ERROR;
    }
    if (empty()) {
      parsed_ = true;
      return ResultParse::kEnd;
    }
    if (back() == kObject) {
      JSON_PARSE_ERROR;
    } else if (back() == kArray) {
      return ResultParse::kContinue;
    } else {
      pop_back();
      return ResultParse::kContinue;
    }
  }
  ResultParse string(std::string_view val) {
    return value();
  }
  ResultParse integral(std::int64_t val) {
    return value();
  }
  ResultParse unsigned_integral(std::uint64_t val) {
    return value();
  }
  ResultParse floating(double val) {
    return value();
  }
  ResultParse boolean(bool val) {
    return value();
  }
  ResultParse null() {
    return value();
  }

  std::array<type, 128> path_;
  std::size_t size_{};
  bool parsed_{};
};

}  // namespace tgbm::json

#endif
