#pragma once

#include "tgbm/jsons/sax.hpp"

namespace tgbm::json {

template <typename T>
struct sax_parser<std::vector<T>> {
  static sax_consumer_t parse(std::vector<T>& v, sax_token& tok) {
    tok.expect(sax_token::array_begin);
    for (;;) {
      co_yield {};
      if (tok.got == sax_token::array_end)
        break;
      co_yield dd::elements_of(sax_parser<T>::parse(v.emplace_back(), tok));
    }
  }
};

}  // namespace tgbm::json
