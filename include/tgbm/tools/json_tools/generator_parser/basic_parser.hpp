#pragma once

#include <tgbm/tools/pfr_extension.hpp>
#include <tgbm/tools/json_tools/exceptions.hpp>
#include <tgbm/logger.h>
#include <kelcoro/generator.hpp>

namespace tgbm::generator_parser {

using nothing_t = dd::nothing_t;

struct event_holder {
  union {
    double double_m;
    int64_t int_m;
    uint64_t uint_m;
    std::string_view key_m;
    std::string_view str_m;
    bool bool_m;
  };

  event_holder() {
  }
  ~event_holder() {
  }

  enum wait_e {
    array_begin = 1,
    array_end = 2,
    object_begin = 4,
    object_end = 8,
    string = 16,
    key = 32,
    int64 = 64,
    uint64 = 128,
    double_ = 256,
    bool_ = 512,
    null = 1024
  };

  static constexpr std::int16_t all = ~0;
  wait_e got;

  void expect(std::int16_t tokens) {
    if (!(got & tokens)) {
      TGBM_JSON_PARSE_ERROR;
    }
  }
};

inline dd::generator<nothing_t> generator_starter(dd::generator<nothing_t> gen, bool& ended) {
  co_yield {};
  co_yield dd::elements_of(gen);
  ended = true;
  co_yield {};
  TGBM_JSON_PARSE_ERROR;
}

template <typename T>
struct boost_domless_parser {
  static constexpr bool simple = false;
  // dd::generator<nothing_t> parse(T& t_, event_holder& holder)
  // void simple_parse(T& t_, event_holder& holder)
};

template <typename T>
constexpr bool is_simple = requires{
  requires boost_domless_parser<T>::simple;
};

}  // namespace tgbm::generator_parser
