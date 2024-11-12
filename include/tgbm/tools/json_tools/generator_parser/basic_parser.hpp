#pragma once

#include "tgbm/tools/json_tools/generator_parser/stack_memory_resource.hpp"
#include "tgbm/tools/pfr_extension.hpp"
#include "tgbm/tools/json_tools/exceptions.hpp"
#include "tgbm/logger.hpp"

#include <kelcoro/generator.hpp>

namespace tgbm::sax {

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

  enum wait_e : uint16_t {
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
    if (!(got & tokens))
      TGBM_JSON_PARSE_ERROR;
  }
};

struct alignas(dd::coroframe_align()) memory_and_resource {
  byte_t bytes[8096] = {};
  stack_resource res;

  constexpr memory_and_resource() : bytes(), res(bytes) {
  }
};

using parser_t = dd::generator<dd::nothing_t>;

template <typename>
struct parser {
  // required interface:
  //   static parser_t parse(T& v, event_holder& tok)
};

}  // namespace tgbm::sax
