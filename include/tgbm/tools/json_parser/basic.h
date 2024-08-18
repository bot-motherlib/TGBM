#pragma once

#if 0

#include <cassert>
#include <string_view>
#include <tgbm/tools/pfr_extension.hpp>
#include <tgbm/logger.h>

namespace tgbm::json {

template <typename T>
struct Parser {};

enum struct ResultParse { kContinue, kError, kEnd };

[[noreturn]] inline void throw_err() {
  throw std::runtime_error("failed parse");
}
#define JSON_PARSE_ERROR throw_err()
// JSON_PARSE_ERROR

template <typename T>
struct basic_parser {
  T* t_ = nullptr;
  bool parsed_{};

  static constexpr bool required = true;

  basic_parser(T& t) : t_(std::addressof(t)) {
  }

  basic_parser(T* t = nullptr) : t_(t) {
  }

  void on_start_parsing() {
  }

  void rebind(T* t) {
    t_ = t;
    parsed_ = false;
  }
  ResultParse start_object() {
    JSON_PARSE_ERROR;
  }
  ResultParse end_object() {
    JSON_PARSE_ERROR;
  }
  ResultParse start_array() {
    JSON_PARSE_ERROR;
  }
  ResultParse end_array() {
    JSON_PARSE_ERROR;
  }
  ResultParse key(std::string_view val) {
    JSON_PARSE_ERROR;
  }
  ResultParse string(std::string_view val) {
    JSON_PARSE_ERROR;
  }
  ResultParse integral(std::int64_t val) {
    JSON_PARSE_ERROR;
  }
  ResultParse unsigned_integral(std::uint64_t val) {
    JSON_PARSE_ERROR;
  }
  ResultParse floating(double val) {
    JSON_PARSE_ERROR;
  }
  ResultParse boolean(bool val) {
    JSON_PARSE_ERROR;
  }
  ResultParse null() {
    JSON_PARSE_ERROR;
  }
};

template <>
struct basic_parser<void> {
  bool parsed_{};

  basic_parser() = default;

  ResultParse start_object() {
    JSON_PARSE_ERROR;
  }
  ResultParse end_object() {
    JSON_PARSE_ERROR;
  }
  ResultParse start_array() {
    JSON_PARSE_ERROR;
  }
  ResultParse end_array() {
    JSON_PARSE_ERROR;
  }
  ResultParse key(std::string_view val) {
    JSON_PARSE_ERROR;
  }
  ResultParse string(std::string_view val) {
    JSON_PARSE_ERROR;
  }
  ResultParse integral(std::int64_t val) {
    JSON_PARSE_ERROR;
  }
  ResultParse unsigned_integral(std::uint64_t val) {
    JSON_PARSE_ERROR;
  }
  ResultParse floating(double val) {
    JSON_PARSE_ERROR;
  }
  ResultParse boolean(bool val) {
    JSON_PARSE_ERROR;
  }
  ResultParse null() {
    JSON_PARSE_ERROR;
  }
};

template <typename T>
struct parser {};

}  // namespace tgbm::json

#endif
