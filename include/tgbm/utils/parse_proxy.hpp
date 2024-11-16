#pragma once

#include <tgbm/jsons/sax.hpp>

namespace tgbm {

// used for parsing by reference in struct
template <typename T>
struct parse_proxy {
  T& out;

  parse_proxy(T& o) noexcept : out(o) {
  }
};

}  // namespace tgbm

namespace tgbm::json {

template <typename T>
struct sax_parser<parse_proxy<T>> {
  static sax_consumer_t parse(parse_proxy<T>& out, sax_token& tok) {
    return sax_parser<T>::parse(out.out, tok);
  }
};

}  // namespace tgbm::json
