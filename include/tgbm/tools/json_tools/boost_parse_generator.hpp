#pragma once

#include <boost/json/basic_parser_impl.hpp>

#include "tgbm/net/errors.hpp"
#include "tgbm/tools/json_tools/generator_parser/basic_parser.hpp"
#include "tgbm/tools/scope_exit.h"

namespace tgbm::sax {

struct json_tokenizer {
  static constexpr size_t max_array_size = -1;
  static constexpr size_t max_object_size = -1;
  static constexpr size_t max_string_size = -1;
  static constexpr size_t max_key_size = -1;

 private:
  using wait_e = sax::event_holder::wait_e;
  using parser_t = sax::parser_t;

  parser_t gen;
  sax::event_holder event{};
  // must be outside unite generator for string view
  std::string partbuf;
  bool ended = false;
  bool part = false;

  parser_t unite_generate(parser_t old_gen) {
    event.expect(event.part);
    part = true;
    partbuf.clear();
    on_scope_failure(destroy_gen) {
      // guarantee order of deallocactions for resources, firstly this generator, then old_gen
      gen = std::move(old_gen);
    };
    while (event.got == event.part) {
      partbuf += event.str_m;
      co_yield {};
    }
    partbuf += event.str_m;
    event.str_m = partbuf;
    destroy_gen.no_longer_needed();
    (void)gen.release();
    gen = std::move(old_gen);

    part = false;

    co_await dd::this_coro::destroy_and_transfer_control_to(gen.raw_handle().promise().current_worker);
  }

  template <typename T>
  parser_t starter(T& v) {
    co_yield dd::elements_of(sax::parser<T>::parse(v, event));
    ended = true;
    co_yield {};
    TGBM_JSON_PARSE_ERROR;
  }

  void on_part(std::string_view s) {
    event.got = sax::event_holder::part;
    event.str_m = s;
    if (!part)
      gen = unite_generate(std::move(gen));
    resume_generator();
  }

 public:
  json_tokenizer(auto& v) : gen(starter(v)) {
  }

  json_tokenizer(json_tokenizer&&) = delete;

  bool on_document_begin(io_error_code&) {
    return true;
  }

  bool on_document_end(io_error_code&) {
    return true;
  }

  void resume_generator() {
    ++gen.cur_iterator();
  }

  bool on_array_begin(io_error_code&) {
    event.got = wait_e::array_begin;
    resume_generator();
    return true;
  }

  bool on_array_end(size_t n, io_error_code&) {
    event.got = wait_e::array_end;
    resume_generator();
    return true;
  }

  bool on_object_begin(io_error_code&) {
    event.got = wait_e::object_begin;
    resume_generator();
    return true;
  }

  bool on_object_end(size_t n, io_error_code&) {
    event.got = wait_e::object_end;
    resume_generator();
    return true;
  }

  bool on_string(std::string_view s, size_t, io_error_code&) {
    event.got = wait_e::string;
    event.str_m = s;
    resume_generator();
    return true;
  }

  bool on_key(std::string_view s, size_t, io_error_code&) {
    event.got = wait_e::key;
    event.str_m = s;
    resume_generator();
    return true;
  }

  bool on_int64(int64_t i, std::string_view, io_error_code&) {
    event.got = wait_e::int64;
    event.int_m = i;
    resume_generator();
    return true;
  }

  bool on_uint64(uint64_t u, std::string_view, io_error_code&) {
    event.got = wait_e::uint64;
    event.uint_m = u;
    resume_generator();
    return true;
  }

  bool on_double(double d, std::string_view, io_error_code&) {
    event.got = wait_e::double_;
    event.double_m = d;
    resume_generator();
    return true;
  }

  bool on_bool(bool b, io_error_code&) {
    event.got = wait_e::bool_;
    event.bool_m = b;
    resume_generator();
    return true;
  }

  bool on_null(io_error_code&) {
    event.got = wait_e::null;
    resume_generator();
    return true;
  }

  bool on_string_part(std::string_view s, size_t, io_error_code&) {
    on_part(s);
    return true;
  }

  bool on_comment_part(std::string_view, io_error_code&) {
    return true;
  }

  bool on_key_part(std::string_view s, size_t, io_error_code&) {
    on_part(s);
    return true;
  }

  bool on_number_part(std::string_view, io_error_code&) {
    return true;
  }

  bool on_comment(std::string_view, io_error_code&) {
    return true;
  }

  // value parsed already
  [[nodiscard]] bool is_done() const noexcept {
    return ended;
  }
};

}  // namespace tgbm::sax
