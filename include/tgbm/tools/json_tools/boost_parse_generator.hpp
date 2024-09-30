#pragma once
#include <tgbm/tools/json_tools/generator_parser/basic_parser.hpp>
#include <boost/json/basic_parser.hpp>

namespace tgbm::json::boost {
namespace details {

struct wait_handler {
  using error_code = ::boost::json::error_code;
  using string_view = ::boost::json::string_view;

  static constexpr std::size_t max_array_size = -1;
  static constexpr std::size_t max_object_size = -1;
  static constexpr std::size_t max_string_size = -1;
  static constexpr std::size_t max_key_size = -1;

  using wait_e = generator_parser::event_holder::wait_e;

  generator_parser::event_holder event{};
  bool ended{};
  dd::generator<generator_parser::nothing_t> gen;

  template <typename T>
  wait_handler(T& t_)
      : gen(generator_parser::generator_starter(generator_parser::boost_domless_parser<T>::parse(t_, event), ended)) {
    gen.begin();
  }

  wait_handler(wait_handler&&) = delete;

  bool on_document_begin(error_code& ec) {
    return true;
  }

  bool on_document_end(error_code& ec) {
    return true;
  }

  void resume_generator() {
    auto it = dd::generator_iterator<generator_parser::nothing_t>(gen);
    ++it;
  }

  bool on_array_begin(error_code& ec) {
    event.got = wait_e::array_begin;
    resume_generator();
    return true;
  }

  bool on_array_end(std::size_t n, error_code& ec) {
    event.got = wait_e::array_end;
    resume_generator();
    return true;
  }

  bool on_object_begin(error_code& ec) {
    event.got = wait_e::object_begin;
    resume_generator();
    return true;
  }

  bool on_object_end(std::size_t n, error_code& ec) {
    event.got = wait_e::object_end;
    resume_generator();
    return true;
  }

  bool on_string(string_view s, std::size_t n, error_code& ec) {
    event.got = wait_e::string;
    event.str_m = s;
    resume_generator();
    return true;
  }

  bool on_key(string_view s, std::size_t n, error_code& ec) {
    event.got = wait_e::key;
    event.key_m = s;
    resume_generator();
    return true;
  }

  bool on_int64(int64_t i, string_view s, error_code& ec) {
    event.got = wait_e::int64;
    event.int_m = i;
    resume_generator();
    return true;
  }

  bool on_uint64(uint64_t u, string_view s, error_code& ec) {
    event.got = wait_e::uint64;
    event.uint_m = u;
    resume_generator();
    return true;
  }

  bool on_double(double d, string_view s, error_code& ec) {
    event.got = wait_e::double_;
    event.double_m = d;
    resume_generator();
    return true;
  }

  bool on_bool(bool b, error_code& ec) {
    event.got = wait_e::bool_;
    event.bool_m = b;
    resume_generator();
    return true;
  }

  bool on_null(error_code& ec) {
    event.got = wait_e::null;
    resume_generator();
    return true;
  }

  bool on_string_part(string_view s, std::size_t n, error_code& ec) {
    TGBM_JSON_PARSE_ERROR;
    return false;
  }

  bool on_comment_part(string_view s, error_code& ec) {
    TGBM_JSON_PARSE_ERROR;
    return false;
  }

  bool on_key_part(string_view s, std::size_t n, error_code& ec) {
    TGBM_JSON_PARSE_ERROR;
    return false;
  }

  bool on_number_part(string_view s, error_code& ec) {
    TGBM_JSON_PARSE_ERROR;
    return false;
  }

  bool on_comment(string_view s, error_code& ec) {
    return true;
  }
};

}  // namespace details

template <typename T>
T parse_generator(std::string_view data) {
  T t;
  ::boost::json::basic_parser<details::wait_handler> p{::boost::json::parse_options{}, t};
  ::boost::json::error_code ec;
  p.write_some(false, data.data(), data.size(), ec);
  if (ec || !p.handler().ended) {
    TGBM_JSON_PARSE_ERROR;
  }
  return t;
}

}  // namespace tgbm::json::boost
