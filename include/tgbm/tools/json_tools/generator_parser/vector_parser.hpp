#pragma once

#include "tgbm/tools/json_tools/generator_parser/basic_parser.hpp"

namespace tgbm::generator_parser {

template <typename T>
struct boost_domless_parser<std::vector<T>> {
  static dd::generator<nothing_t> parse(std::vector<T>& v, tgbm::generator_parser::event_holder& tok,
                                        memres_tag auto resource) {
    tok.expect(event_holder::array_begin);
    for (;;) {
      co_yield {};
      if (tok.got == event_holder::array_end)
        break;
      co_yield dd::elements_of(boost_domless_parser<T>::parse(v.emplace_back(), tok, resource));
    }
  }
};

}  // namespace tgbm::generator_parser
