#pragma once

#include <cassert>
#include <concepts>
#include <string_view>
#include <vector>

#include <anyany/anyany.hpp>

#include <boost/container/static_vector.hpp>

#include <tgbm/api/const_string.hpp>
#include <tgbm/jsons/errors.hpp>
#include <tgbm/utils/macro.hpp>

#include <tgbm/utils/anyany_utils.hpp>
#include <boost/container/small_vector.hpp>

namespace tgbm::json::handler_parser {

template <typename T>
struct Parser {};

#ifndef TGBM_STATIC_PARSER_SIZE
  #define TGBM_STATIC_PARSER_SIZE 64
#endif

enum struct ResultParse { kContinue, kError, kEnd };

#define TGBM_JSON_HANDLER_PARSE_ERROR return ResultParse::kError
// TGBM_JSON_HANDLER_PARSE_ERROR

namespace details {

ANYANY_METHOD(start_object, ResultParse());
ANYANY_METHOD(end_object, ResultParse());
ANYANY_METHOD(start_array, ResultParse());
ANYANY_METHOD(end_array, ResultParse());
ANYANY_METHOD(key, ResultParse(std::string_view));
ANYANY_METHOD(string, ResultParse(std::string_view));
ANYANY_METHOD(integral, ResultParse(int64_t));
ANYANY_METHOD(unsigned_integral, ResultParse(uint64_t));
ANYANY_METHOD(floating, ResultParse(double));
ANYANY_METHOD(boolean, ResultParse(bool));
ANYANY_METHOD(null, ResultParse());
ANYANY_METHOD(on_next_end_parsing, ResultParse());

template <size_t Size>
using ErasedParser =
    aa::basic_any_with<aa::unreachable_allocator, Size, aa::move, start_object_m, end_object_m, start_array_m,
                       end_array_m, key_m, string_m, integral_m, unsigned_integral_m, floating_m, boolean_m,
                       null_m, on_next_end_parsing_m>;

}  // namespace details

using ErasedParser = details::ErasedParser<TGBM_STATIC_PARSER_SIZE>;

struct ParserStack {
  ::boost::container::static_vector<ErasedParser, 64> stack_;

 public:
  void push(ErasedParser parser) {
    stack_.push_back(std::move(parser));
  }

  void pop() {
    assert(!stack_.empty());
    stack_.pop_back();
  }

  bool empty() {
    return stack_.empty();
  }

  ErasedParser& last() {
    assert(!stack_.empty());

    return stack_.back();
  }

  ResultParse handle_result(ResultParse res) {
    while (res == ResultParse::kEnd && !stack_.empty()) {
      pop();
      if (!stack_.empty())
        res = on_next_end_parsing();
    }
    return res;
  }

  ResultParse on_next_end_parsing() {
    return last().on_next_end_parsing();
  }

  bool start_object() {
    auto res = last().start_object();
    res = handle_result(res);
    return res != ResultParse::kError;
  }
  bool end_object() {
    auto res = last().end_object();
    res = handle_result(res);
    return res != ResultParse::kError;
  }
  bool start_array() {
    auto res = last().start_array();
    res = handle_result(res);
    return res != ResultParse::kError;
  }
  bool end_array() {
    auto res = last().end_array();
    res = handle_result(res);
    return res != ResultParse::kError;
  }
  bool key(std::string_view val) {
    auto res = last().key(val);
    res = handle_result(res);
    return res != ResultParse::kError;
  }
  bool string(std::string_view val) {
    auto res = last().string(val);
    res = handle_result(res);
    return res != ResultParse::kError;
  }
  bool string(const tgbm::const_string& s) {
    return string(s.str());
  }
  bool integral(std::int64_t val) {
    auto res = last().integral(val);
    res = handle_result(res);
    return res != ResultParse::kError;
  }
  bool unsigned_integral(std::uint64_t val) {
    auto res = last().unsigned_integral(val);
    res = handle_result(res);
    return res != ResultParse::kError;
  }
  bool floating(double val) {
    auto res = last().floating(val);
    res = handle_result(res);
    return res != ResultParse::kError;
  }
  bool boolean(bool val) {
    auto res = last().boolean(val);
    res = handle_result(res);
    return res != ResultParse::kError;
  }
  bool null() {
    auto res = last().null();
    res = handle_result(res);
    return res != ResultParse::kError;
  }
};

template <typename T>
struct basic_parser {
  ParserStack& parser_stack_;
  T* t_ = nullptr;

  basic_parser(ParserStack& stack, T& t) : parser_stack_(stack), t_(std::addressof(t)) {
  }

  basic_parser(ParserStack& stack, T* t = nullptr) : parser_stack_(stack), t_(t) {
  }

  ResultParse on_next_end_parsing() {
    return ResultParse::kContinue;
  }

  ResultParse start_object() {
    TGBM_JSON_HANDLER_PARSE_ERROR;
  }
  ResultParse end_object() {
    TGBM_JSON_HANDLER_PARSE_ERROR;
  }
  ResultParse start_array() {
    TGBM_JSON_HANDLER_PARSE_ERROR;
  }
  ResultParse end_array() {
    TGBM_JSON_HANDLER_PARSE_ERROR;
  }
  ResultParse key(std::string_view val) {
    TGBM_JSON_HANDLER_PARSE_ERROR;
  }
  ResultParse string(std::string_view val) {
    TGBM_JSON_HANDLER_PARSE_ERROR;
  }
  ResultParse integral(std::int64_t val) {
    TGBM_JSON_HANDLER_PARSE_ERROR;
  }
  ResultParse unsigned_integral(std::uint64_t val) {
    TGBM_JSON_HANDLER_PARSE_ERROR;
  }
  ResultParse floating(double val) {
    TGBM_JSON_HANDLER_PARSE_ERROR;
  }
  ResultParse boolean(bool val) {
    TGBM_JSON_HANDLER_PARSE_ERROR;
  }
  ResultParse null() {
    TGBM_JSON_HANDLER_PARSE_ERROR;
  }
};

template <>
struct basic_parser<void> {
  basic_parser() = default;
  void on_start_parsing() {
  }

  ResultParse start_object() {
    TGBM_JSON_HANDLER_PARSE_ERROR;
  }
  ResultParse end_object() {
    TGBM_JSON_HANDLER_PARSE_ERROR;
  }
  ResultParse start_array() {
    TGBM_JSON_HANDLER_PARSE_ERROR;
  }
  ResultParse end_array() {
    TGBM_JSON_HANDLER_PARSE_ERROR;
  }
  ResultParse key(std::string_view val) {
    TGBM_JSON_HANDLER_PARSE_ERROR;
  }
  ResultParse string(std::string_view val) {
    TGBM_JSON_HANDLER_PARSE_ERROR;
  }
  ResultParse integral(std::int64_t val) {
    TGBM_JSON_HANDLER_PARSE_ERROR;
  }
  ResultParse unsigned_integral(std::uint64_t val) {
    TGBM_JSON_HANDLER_PARSE_ERROR;
  }
  ResultParse floating(double val) {
    TGBM_JSON_HANDLER_PARSE_ERROR;
  }
  ResultParse boolean(bool val) {
    TGBM_JSON_HANDLER_PARSE_ERROR;
  }
  ResultParse null() {
    TGBM_JSON_HANDLER_PARSE_ERROR;
  }
};

template <typename T>
struct parser {
  parser(ParserStack& stack, T& t) {
    static_assert(!std::same_as<T, T>);
  }
};

}  // namespace tgbm::json::handler_parser
