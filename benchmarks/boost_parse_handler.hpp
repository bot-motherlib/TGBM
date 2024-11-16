#pragma once

#include <boost/json/basic_parser_impl.hpp>

#include "handler_parser/basic_parser.hpp"
#include <tgbm/jsons/errors.hpp>

namespace tgbm {
using io_error_code = boost::system::error_code;
}
namespace tgbm::json::boost {

namespace details {

struct ignore_handler {
  static constexpr size_t max_array_size = -1;
  static constexpr size_t max_object_size = -1;
  static constexpr size_t max_string_size = -1;
  static constexpr size_t max_key_size = -1;

  bool on_document_begin(io_error_code& ec) {
    return true;
  }

  bool on_document_end(io_error_code& ec) {
    return true;
  }

  bool on_array_begin(io_error_code& ec) {
    return true;
  }

  bool on_array_end(size_t n, io_error_code& ec) {
    return true;
  }

  bool on_object_begin(io_error_code& ec) {
    return true;
  }

  bool on_object_end(size_t n, io_error_code& ec) {
    return true;
  }

  bool on_string_part(std::string_view s, size_t n, io_error_code& ec) {
    ec = ::boost::json::error::incomplete;
    return false;
  }

  bool on_string(std::string_view s, size_t n, io_error_code& ec) {
    return true;
  }

  bool on_key_part(std::string_view s, size_t n, io_error_code& ec) {
    ec = ::boost::json::error::incomplete;
    return false;
  }

  bool on_key(std::string_view s, size_t n, io_error_code& ec) {
    return true;
  }

  bool on_number_part(std::string_view s, io_error_code& ec) {
    ec = ::boost::json::error::incomplete;
    return false;
  }

  bool on_int64(int64_t i, std::string_view s, io_error_code& ec) {
    return true;
  }

  bool on_uint64(uint64_t u, std::string_view s, io_error_code& ec) {
    return true;
  }

  bool on_double(double d, std::string_view s, io_error_code& ec) {
    return true;
  }

  bool on_bool(bool b, io_error_code& ec) {
    return true;
  }

  bool on_null(io_error_code& ec) {
    return true;
  }

  bool on_comment_part(std::string_view, io_error_code& ec) {
    ec = ::boost::json::error::incomplete;
    return false;
  }

  bool on_comment(std::string_view, io_error_code&) {
    return true;
  }
};

template <typename T>
struct parser_handler {
  using string_view = ::boost::json::string_view;
  static constexpr size_t max_array_size = -1;
  static constexpr size_t max_object_size = -1;
  static constexpr size_t max_string_size = -1;
  static constexpr size_t max_key_size = -1;

  parser_handler(T& t) {
    stack_.push(handler_parser::parser<T>{stack_, t});
  }

  bool on_document_begin(io_error_code& ec) {
    return true;
  }

  bool on_document_end(io_error_code& ec) {
    return true;
  }

  bool on_array_begin(io_error_code& ec) {
    return stack_.start_array();
  }

  bool on_array_end(size_t n, io_error_code& ec) {
    return stack_.end_array();
  }

  bool on_object_begin(io_error_code& ec) {
    return stack_.start_object();
  }

  bool on_object_end(size_t n, io_error_code& ec) {
    return stack_.end_object();
  }

  bool on_string_part(string_view s, size_t n, io_error_code& ec) {
    ec = ::boost::json::error::incomplete;
    return false;
  }

  bool on_string(string_view s, size_t n, io_error_code& ec) {
    return stack_.string(s);
  }

  bool on_key_part(string_view s, size_t n, io_error_code& ec) {
    ec = ::boost::json::error::incomplete;
    return false;
  }

  bool on_key(string_view s, size_t n, io_error_code& ec) {
    return stack_.key(s);
  }

  bool on_number_part(string_view s, io_error_code& ec) {
    ec = ::boost::json::error::incomplete;
    return false;
  }

  bool on_int64(int64_t i, string_view s, io_error_code& ec) {
    return stack_.integral(i);
  }

  bool on_uint64(uint64_t u, string_view s, io_error_code& ec) {
    return stack_.unsigned_integral(u);
  }

  bool on_double(double d, string_view s, io_error_code& ec) {
    return stack_.floating(d);
  }

  bool on_bool(bool b, io_error_code& ec) {
    return stack_.boolean(b);
  }

  bool on_null(io_error_code& ec) {
    return stack_.null();
  }

  bool on_comment_part(string_view s, io_error_code& ec) {
    ec = ::boost::json::error::incomplete;
    return false;
  }

  bool on_comment(string_view s, io_error_code& ec) {
    return true;
  }

 private:
  handler_parser::ParserStack stack_;
};

}  // namespace details

template <typename T>
T parse_handler(std::string_view sv) {
  T result;
  details::parser_handler<T> handler(result);
  ::boost::json::basic_parser<details::parser_handler<T>> parser(::boost::json::parse_options{}, result);
  io_error_code ec;
  parser.write_some(false, sv.data(), sv.size(), ec);
  if (ec.failed()) {
    TGBM_JSON_PARSE_ERROR;
  }
  return result;
}

inline bool parse_ignore_handler(std::string_view sv) {
  ::boost::json::basic_parser<details::ignore_handler> parser(::boost::json::parse_options{});
  io_error_code ec;
  parser.write_some(false, sv.data(), sv.size(), ec);
  if (ec.failed()) {
    TGBM_JSON_PARSE_ERROR;
  }
  return true;
}

}  // namespace tgbm::json::boost
