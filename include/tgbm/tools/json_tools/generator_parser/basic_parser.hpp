#pragma once

#include "tgbm/tools/pfr_extension.hpp"
#include "tgbm/tools/json_tools/exceptions.hpp"
#include "tgbm/logger.hpp"

#include <kelcoro/generator.hpp>

namespace tgbm::generator_parser {

using nothing_t = dd::nothing_t;

struct event_holder {
  union {
    double double_m;
    int64_t int_m;
    uint64_t uint_m;
    std::string_view str_m;
    bool bool_m;
  };

  event_holder() {
  }
  ~event_holder() {
  }

  enum wait_e {
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
  wait_e got;

  void expect(std::int16_t tokens) const {
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
constexpr bool is_simple = requires { requires boost_domless_parser<T>::simple; };

}  // namespace tgbm::generator_parser
