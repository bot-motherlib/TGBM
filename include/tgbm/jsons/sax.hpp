#pragma once

#include <tgbm/utils/pfr_extension.hpp>
#include <tgbm/jsons/errors.hpp>

#include <kelcoro/generator.hpp>

namespace tgbm::json {

struct sax_token {
  union {
    double double_m;
    int64_t int_m;
    uint64_t uint_m;
    std::string_view str_m;
    bool bool_m;
  };

  sax_token() {
  }
  ~sax_token() {
  }

  enum kind_e : uint16_t {
    array_begin = 1 << 0,
    array_end = 1 << 1,
    object_begin = 1 << 2,
    object_end = 1 << 3,
    string = 1 << 4,
    key = 1 << 5,
    int64 = 1 << 6,
    uint64 = 1 << 7,
    double_ = 1 << 8,
    bool_ = 1 << 9,
    null = 1 << 10,
    part = 1 << 11,
  };

  static constexpr std::int16_t all = ~0;
  kind_e got;

  void expect(std::int16_t tokens) const {
    if (!(got & tokens))
      TGBM_JSON_PARSE_ERROR;
  }
};

using sax_consumer_t = dd::generator<dd::nothing_t>;

template <typename>
struct sax_parser {
  // required interface:
  //   static sax_consumer_t parse(T& v, sax_token& tok)
  // Note: 'tok' never will be 'part', its handled by token producer
};

inline sax_consumer_t sax_ignore_value(sax_token& tok) {
  switch (tok.got) {
    case sax_token::string:
    case sax_token::int64:
    case sax_token::uint64:
    case sax_token::double_:
    case sax_token::bool_:
    case sax_token::null:
      co_return;
    default:
      break;
  }
  size_t depth = 0;
  for (;;) {
    switch (tok.got) {
      case sax_token::array_begin:
      case sax_token::object_begin:
        depth++;
        co_yield {};
        break;
      case sax_token::part:
        unreachable();
      case sax_token::array_end:
      case sax_token::object_end:
        depth--;
        if (depth == 0)
          co_return;
      case sax_token::string:
      case sax_token::int64:
      case sax_token::uint64:
      case sax_token::double_:
      case sax_token::bool_:
      case sax_token::null:
      case sax_token::key:
        co_yield {};
    }
  }
}

}  // namespace tgbm::json
