#pragma once

#include "tgbm/tools/json_tools/generator_parser/basic_parser.hpp"

namespace tgbm {

// used for parsing by reference in struct
template <typename T>
struct parse_proxy {
  T& out;

  parse_proxy(T& o) noexcept : out(o) {
  }
};

}  // namespace tgbm

namespace tgbm::json::sax {

template <typename T>
struct parser<parse_proxy<T>> {
  static parser_t parse(parse_proxy<T>& out, event_holder& tok) {
    return parser<T>::parse(out.out, tok);
  }
};

}  // namespace tgbm::json::sax
