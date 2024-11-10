#pragma once

#include "tgbm/api/const_string.hpp"
#include "tgbm/api/types/all.hpp"

#include "tgbm/tools/box_union.hpp"

#include "tgbm/tools/json_tools/generator_parser/all.hpp"

namespace tgbm::api {

// used for telegram answer parsing (tgbm::from_json)
// accepts expected result and parses
// { "ok": <bool>, ?"error_code": <int>?, ?"descripion": <string>?, ?"result": <T>"
template <typename T>
struct telegram_answer {
  bool ok = false;
  T& result;
  const_string description;
  int error_code = -1;

  explicit telegram_answer(T& out) noexcept : result(out) {
  }
};

}  // namespace tgbm::api

namespace tgbm::generator_parser {

template <typename T>
struct boost_domless_parser<api::telegram_answer<T>> {
  static dd::generator<dd::nothing_t> parse(api::telegram_answer<T>& out, event_holder& tok) {
    using enum event_holder::wait_e;
    tok.expect(object_begin);

    // ignore "error_code"
    enum key_e {
      ok_key = 1 << 0,
      description_key = 1 << 1,
      result_key = 1 << 2,
      unknown_key = 1 << 3,
    };
    int8_t parsed = 0;
    for (;;) {
      co_yield {};

      if (tok.got == object_end) [[unlikely]]
        break;
      tok.expect(key);
      int8_t curkey = utils::string_switch<key_e>(tok.str_m)
                          .case_("ok", ok_key)
                          .case_("result", result_key)
                          .case_("description", description_key)
                          .or_default(unknown_key);
      if (curkey != unknown_key) {
        if (parsed & curkey)
          json::throw_json_parse_error();
        parsed |= curkey;
      }
      co_yield {};
      switch (curkey) {
        case ok_key:
          tok.expect(bool_);
          out.ok = tok.bool_m;
          continue;
        case result_key:
          co_yield dd::elements_of(boost_domless_parser<T>::parse(out.result, tok));
          continue;
        case description_key:
          tok.expect(string);
          out.description = tok.str_m;
          continue;
        case unknown_key:
          co_yield dd::elements_of(ignore_parser::parse(tok));
          continue;
      }
    }
    if (!(parsed & ok_key))  // only mandatory field
      json::throw_json_parse_error();
    co_return;
  }
};

// for parsing return type of some operations
template <>
struct boost_domless_parser<tgbm::box_union<bool, tgbm::api::Message>> {
  static dd::generator<dd::nothing_t> parse(tgbm::box_union<bool, tgbm::api::Message>& out,
                                            event_holder& tok) {
    using enum event_holder::wait_e;
    if (tok.got == bool_) {
      out = tok.bool_m;
      return {};
    }
    return boost_domless_parser<api::Message>::parse(out.emplace<tgbm::api::Message>(), tok);
  }
};

}  // namespace tgbm::generator_parser
