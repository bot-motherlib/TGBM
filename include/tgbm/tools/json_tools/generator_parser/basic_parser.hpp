#pragma once

#include <tgbm/tools/pfr_extension.hpp>
#include <tgbm/tools/json_tools/exceptions.hpp>
#include <tgbm/logger.h>
#include <kelcoro/generator.hpp>
#include "tgbm/tools/stack_resource.hpp"

namespace tgbm::generator_parser {

using nothing_t = dd::nothing_t;

using generator = dd::generator<nothing_t>;

struct stack_resource_holder{
  stack_resource* resource;

  stack_resource_holder(stack_resource& r) : resource(&r){}

  void* allocate(std::size_t bytes, std::size_t alignment){
    return resource->allocate(bytes, alignment);
  }
  void deallocate(void* ptr, std::size_t bytes, std::size_t alignment) noexcept{
    resource->deallocate(ptr, bytes, alignment);
  }
};

using with_pmr = dd::with_resource<stack_resource_holder>;

// template <typename R>

struct event_holder {
  union {
    double double_m;
    int64_t int_m;
    uint64_t uint_m;
    std::string_view str_m;
    bool bool_m;
  };

  event_holder() noexcept {
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
    null = 1024,
    part = 2048
  };

  static constexpr std::int16_t all = ~0;
  wait_e got;

  void expect(std::int16_t tokens) {
    if (!(got & tokens)) {
      TGBM_JSON_PARSE_ERROR;
    }
  }
};

template <typename T>
struct boost_domless_parser {
  static constexpr bool simple = false;
  // generator parse(T& t_, event_holder& holder, with_pmr r)
  // void simple_parse(T& t_, event_holder& holder)
};

template <typename T>
constexpr bool is_simple = requires { requires boost_domless_parser<T>::simple; };

}  // namespace tgbm::generator_parser

namespace dd{
template <>
struct with_resource<tgbm::generator_parser::stack_resource_holder>{
  KELCORO_NO_UNIQUE_ADDRESS tgbm::generator_parser::stack_resource_holder resource;

  with_resource(tgbm::stack_resource& mr) noexcept : resource(mr) {
  }
};

with_resource(tgbm::stack_resource&) -> with_resource<tgbm::generator_parser::stack_resource_holder>;

}