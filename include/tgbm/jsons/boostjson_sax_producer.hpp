#pragma once

#include <boost/json/basic_parser_impl.hpp>

#include "tgbm/jsons/sax.hpp"
#include "tgbm/utils/scope_exit.hpp"

namespace tgbm {
using io_error_code = boost::system::error_code;
}

namespace tgbm::json {

struct boostjson_sax_producer {
  static constexpr size_t max_array_size = -1;
  static constexpr size_t max_object_size = -1;
  static constexpr size_t max_string_size = -1;
  static constexpr size_t max_key_size = -1;

 private:
  using kind_e = sax_token::kind_e;

  sax_consumer_t gen;
  sax_token event{};
  // must be outside unite generator for string view
  std::string partbuf;
  bool ended = false;
  bool part = false;

  sax_consumer_t unite_generate(sax_consumer_t old_gen) {
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
  sax_consumer_t starter(T& v) {
    co_yield dd::elements_of(sax_parser<T>::parse(v, event));
    ended = true;
    co_yield {};
    TGBM_JSON_PARSE_ERROR;
  }

  void on_part(std::string_view s) {
    event.got = sax_token::part;
    event.str_m = s;
    if (!part)
      gen = unite_generate(std::move(gen));
    consume();
  }

  void consume() {
    ++gen.cur_iterator();
  }

 public:
  explicit boostjson_sax_producer(auto& v) : gen(starter(v)) {
  }

  boostjson_sax_producer(boostjson_sax_producer&&) = delete;
  void operator=(boostjson_sax_producer&&) = delete;

  bool on_document_begin(io_error_code&) {
    return true;
  }

  bool on_document_end(io_error_code&) {
    return true;
  }

  bool on_array_begin(io_error_code&) {
    event.got = kind_e::array_begin;
    consume();
    return true;
  }

  bool on_array_end(size_t n, io_error_code&) {
    event.got = kind_e::array_end;
    consume();
    return true;
  }

  bool on_object_begin(io_error_code&) {
    event.got = kind_e::object_begin;
    consume();
    return true;
  }

  bool on_object_end(size_t n, io_error_code&) {
    event.got = kind_e::object_end;
    consume();
    return true;
  }

  bool on_string(std::string_view s, size_t, io_error_code&) {
    event.got = kind_e::string;
    event.str_m = s;
    consume();
    return true;
  }

  bool on_key(std::string_view s, size_t, io_error_code&) {
    event.got = kind_e::key;
    event.str_m = s;
    consume();
    return true;
  }

  bool on_int64(int64_t i, std::string_view, io_error_code&) {
    event.got = kind_e::int64;
    event.int_m = i;
    consume();
    return true;
  }

  bool on_uint64(uint64_t u, std::string_view, io_error_code&) {
    event.got = kind_e::uint64;
    event.uint_m = u;
    consume();
    return true;
  }

  bool on_double(double d, std::string_view, io_error_code&) {
    event.got = kind_e::double_;
    event.double_m = d;
    consume();
    return true;
  }

  bool on_bool(bool b, io_error_code&) {
    event.got = kind_e::bool_;
    event.bool_m = b;
    consume();
    return true;
  }

  bool on_null(io_error_code&) {
    event.got = kind_e::null;
    consume();
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

}  // namespace tgbm::json
