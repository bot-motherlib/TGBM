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

namespace tgbm::generator_parser {

template <typename T>
struct boost_domless_parser<parse_proxy<T>> {
  dd::generator<dd::nothing_t> parse(parse_proxy<T>& out, event_holder& tok) {
    return boost_domless_parser<T>::parse(out.out, tok);
  }
};

}  // namespace tgbm::generator_parser
