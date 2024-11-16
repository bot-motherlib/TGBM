#pragma once

#include <tgbm/api/const_string.hpp>
#include <tgbm/api/types/all.hpp>

#include <tgbm/utils/box_union.hpp>

#include <tgbm/jsons/sax_parser.hpp>

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

namespace tgbm::json {

template <typename T>
struct sax_parser<api::telegram_answer<T>> {
  static sax_consumer_t parse(api::telegram_answer<T>& out, sax_token& tok) {
    using enum sax_token::kind_e;
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
      int8_t curkey = string_switch<key_e>(tok.str_m)
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
          co_yield dd::elements_of(sax_parser<T>::parse(out.result, tok));
          continue;
        case description_key:
          tok.expect(string);
          out.description = tok.str_m;
          continue;
        case unknown_key:
          co_yield dd::elements_of(sax_ignore_value(tok));
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
struct sax_parser<tgbm::box_union<bool, tgbm::api::Message>> {
  static sax_consumer_t parse(tgbm::box_union<bool, tgbm::api::Message>& out, sax_token& tok) {
    using enum sax_token::kind_e;
    if (tok.got == bool_) {
      out = tok.bool_m;
      return {};
    }
    return sax_parser<api::Message>::parse(out.emplace<tgbm::api::Message>(), tok);
  }
};

}  // namespace tgbm::json
