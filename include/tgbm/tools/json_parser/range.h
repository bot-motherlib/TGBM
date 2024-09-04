#pragma once

#if 0

#include <tgbm/tools/traits.hpp>
#include <tgbm/tools/json_parser/basic.h>
#include <vector>

namespace tgbm::json {
// TODO: string_like
// TODO: range_like
template <string_like T>
struct parser<T> : basic_parser<T> {
  parser(T& t) : basic_parser<T>(t) {
  }
  parser(T* t) : basic_parser<T>(t) {
  }
  void rebind(T* t) {
    this->t_ = t;
  }

  ResultParse string(std::string_view val) {
    assert(!this->parsed_);
    this->parsed_ = true;
    *this->t_ = val;
    return ResultParse::kEnd;
  }
};

template <>
struct parser<std::string_view> {};

template <typename T>
struct parser<std::vector<T>> : basic_parser<std::vector<T>> {
  bool started_{};
  bool parsing_elem_{};
  parser<T> parser_{};

  void rebind(std::vector<T>* t) {
    started_ = {};
    parsing_elem_ = {};
    parser_ = {};
    basic_parser<std::vector<T>>::rebind(t);
  }

  parser(std::vector<T>& t) : basic_parser<T>(t) {
  }

  ResultParse handle_elem_result(ResultParse res) {
    if (res != ResultParse::kEnd) {
      return res;
    } else {
      parsing_elem_ = false;
      return ResultParse::kContinue;
    }
  }

  void CreateAndRebind() {
    auto& cur = this->t_.emplace_back();
    parser_.rebind(std::addressof(cur));
  }

  ResultParse start_object() {
    if (!started_) {
      JSON_PARSE_ERROR;
    }
    if (started_) {
      return parser_.start_object();
    } else {
      CreateAndRebind();
      return parser_.start_object();
    }
  }
  ResultParse end_object() {
    if (parsing_elem_) {
      return handle_elem_result(parser_.end_object());
    }
    return ResultParse::kEnd;
  }
  ResultParse start_array() {
    if (!started_) {
      started_ = true;
      return ResultParse::kContinue;
    }
    return handle_elem_result(parser_.start_array());
  }
  ResultParse end_array() {
    if (parsing_elem_) {
      return handle_elem_result(parser_.end_array());
    }
    return ResultParse::kEnd;
  }
  ResultParse key(std::string_view val) {
    if (parsing_elem_) {
      return parser_.key(val);
    }
    JSON_PARSE_ERROR;
  }
  ResultParse string(std::string_view val) {
    if (parsing_elem_) {
      return handle_elem_result(parser_.string(val));
    }
    JSON_PARSE_ERROR;
  }
  ResultParse integral(std::int64_t val) {
    if (parsing_elem_) {
      return handle_elem_result(parser_.integral(val));
    }
    JSON_PARSE_ERROR;
  }
  ResultParse unsigned_integral(std::uint64_t val) {
    if (parsing_elem_) {
      return handle_elem_result(parser_.unsigned_integral(val));
    }
    JSON_PARSE_ERROR;
  }
  ResultParse floating(double val) {
    if (parsing_elem_) {
      return handle_elem_result(parser_.floating(val));
    }
    JSON_PARSE_ERROR;
  }
  ResultParse boolean(bool val) {
    if (parsing_elem_) {
      return handle_elem_result(parser_.boolean(val));
    }
    JSON_PARSE_ERROR;
  }
  ResultParse null() {
    if (parsing_elem_) {
      return handle_elem_result(parser_.null());
    }
    JSON_PARSE_ERROR;
  }
};
}  // namespace tgbm::json

#endif
