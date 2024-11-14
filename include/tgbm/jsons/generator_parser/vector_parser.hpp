#pragma once

#include "tgbm/jsons/generator_parser/basic_parser.hpp"

namespace tgbm::json::sax {

template <typename T>
struct parser<std::vector<T>> {
  static parser_t parse(std::vector<T>& v, tgbm::json::sax::event_holder& tok) {
    tok.expect(event_holder::array_begin);
    for (;;) {
      co_yield {};
      if (tok.got == event_holder::array_end)
        break;
      co_yield dd::elements_of(parser<T>::parse(v.emplace_back(), tok));
    }
  }
};

}  // namespace tgbm::json::sax
