#pragma once

#include <kelcoro/stack_memory_resource.hpp>

#include <tgbm/jsons/sax.hpp>

namespace tgbm::json {

template <typename T>
struct sax_parser<std::vector<T>> {
  static sax_consumer_t parse(std::vector<T>& v, sax_token& tok, dd::with_stack_resource r) {
    tok.expect(sax_token::array_begin);
    for (;;) {
      co_yield {};
      if (tok.got == sax_token::array_end)
        break;
      co_yield dd::elements_of(sax_parser<T>::parse(v.emplace_back(), tok, r));
    }
  }
};

}  // namespace tgbm::json
