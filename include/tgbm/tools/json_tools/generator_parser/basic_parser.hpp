#pragma once

#include <tgbm/tools/pfr_extension.hpp>
#include <tgbm/tools/json_tools/exceptions.hpp>
#include <tgbm/logger.h>
#include <kelcoro/generator.hpp>

namespace tgbm::generator_parser {

using nothing_t = dd::nothing_t;

using generator = dd::generator<nothing_t>;

struct log_memory_resource : std::pmr::memory_resource{
  std::pmr::memory_resource* resourse = nullptr;

  explicit log_memory_resource(std::pmr::memory_resource* resourse = std::pmr::new_delete_resource()) 
  : resourse(resourse){
    
  }

  void* do_allocate(size_t bytes, size_t alignment) override{
    LOG("{} allocated: {}, {}", (void*)this, bytes, alignment); 
    return resourse->allocate(bytes, alignment);
  }

  void do_deallocate(void *p, size_t bytes, size_t alignment) override{
    LOG("{} deallocated: {}, {}", (void*)this, bytes, alignment); 
    return resourse->deallocate(p, bytes);
  }
  
  bool do_is_equal(const memory_resource &other) const noexcept override{
    return resourse->is_equal(other);
  }
};

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

inline generator generator_starter(generator gen, bool& ended) {
  co_yield {};
  co_yield dd::elements_of(gen);
  ended = true;
  co_yield {};
  TGBM_JSON_PARSE_ERROR;
}

template <typename T>
struct boost_domless_parser {
  static constexpr bool simple = false;
  // generator parse(T& t_, event_holder& holder)
  // void simple_parse(T& t_, event_holder& holder)
};

template <typename T>
constexpr bool is_simple = requires { requires boost_domless_parser<T>::simple; };

}  // namespace tgbm::generator_parser
