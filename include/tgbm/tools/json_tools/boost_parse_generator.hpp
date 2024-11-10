#pragma once

#include <boost/json/basic_parser_impl.hpp>

#include "tgbm/tools/json_tools/generator_parser/basic_parser.hpp"
#include "tgbm/tools/json_tools/generator_parser/stack_memory_resource.hpp"
#include "tgbm/tools/scope_exit.h"

namespace fmt {

template <>
struct formatter<::boost::json::string_view> : formatter<std::string_view> {
  static auto format(::boost::json::string_view sv, auto& ctx) -> decltype(ctx.out()) {
    return fmt::format_to(ctx.out(), "{}", std::string_view{sv.data(), sv.size()});
  }
};

}  // namespace fmt

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

  dd::generator<generator_parser::nothing_t> gen;
  generator_parser::event_holder event{};
  std::vector<char> buf;
  bool ended = false;
  bool part = false;

 private:
  dd::generator<generator_parser::nothing_t> unite_generate(
      dd::generator<generator_parser::nothing_t> old_gen) {
    event.expect(event.part);
    part = true;
    buf.clear();
    on_scope_failure(destroy_gen) {
      // guarantee order of deallocactions for resources, firstly this generator, then old_gen
      gen = std::move(old_gen);
    };
    while (event.got == event.part) {
      buf.insert(buf.end(), event.str_m.begin(), event.str_m.end());
      co_yield {};
    }

    buf.insert(buf.end(), event.str_m.begin(), event.str_m.end());
    event.str_m = std::string_view{buf.data(), buf.size()};
    destroy_gen.no_longer_needed();
    (void)gen.release();
    gen = std::move(old_gen);

    part = false;

    co_await dd::this_coro::destroy_and_transfer_control_to(gen.raw_handle().promise().current_worker);
  }

  template <typename T>
  dd::generator<dd::nothing_t> starter(T& v) {
    co_yield dd::elements_of(generator_parser::boost_domless_parser<T>::parse(v, event));
    ended = true;
    co_yield {};
    TGBM_JSON_PARSE_ERROR;
  }

 public:
  wait_handler(auto& v) : gen(starter(v)) {
    gen.prepare_to_start();
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
    event.str_m = s;
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

  void on_part(string_view s) {
    event.got = generator_parser::event_holder::part;
    event.str_m = s;
    if (!part) {
      gen = unite_generate(std::move(gen));
      gen.prepare_to_start();
    }
    resume_generator();
  }

  bool on_string_part(string_view s, std::size_t n, error_code& ec) {
    on_part(s);
    return true;
  }

  bool on_comment_part(string_view s, error_code& ec) {
    return true;
  }

  bool on_key_part(string_view s, std::size_t n, error_code& ec) {
    on_part(s);
    return true;
  }

  bool on_number_part(string_view s, error_code& ec) {
    return true;
  }

  bool on_comment(string_view s, error_code& ec) {
    return true;
  }
};

}  // namespace details

template <typename T>
T parse_generator(std::string_view data) {
  T v;
  ::boost::json::basic_parser<details::wait_handler> p{::boost::json::parse_options{}, v};
  ::boost::json::error_code ec;
  p.write_some(false, data.data(), data.size(), ec);
  if (ec || !p.handler().ended)
    TGBM_JSON_PARSE_ERROR;
  return v;
}

}  // namespace tgbm::json::boost
