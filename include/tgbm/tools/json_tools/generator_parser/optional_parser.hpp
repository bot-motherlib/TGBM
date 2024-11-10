#pragma once

#include "tgbm/tools/json_tools/generator_parser/basic_parser.hpp"
#include "tgbm/api/optional.hpp"
#include "tgbm/tools/box.hpp"

namespace tgbm::generator_parser {

template <typename Optional>
struct basic_optional_parser {
  using T = typename Optional::value_type;

  static dd::generator<nothing_t> parse(Optional& v, event_holder& tok) {
    if (tok.got == event_holder::null) [[unlikely]]
      return {};
    return boost_domless_parser<T>::parse(v.emplace(), tok);
  }
};

template <typename T>
struct boost_domless_parser<std::optional<T>> : basic_optional_parser<std::optional<T>> {};

template <typename T>
struct boost_domless_parser<api::optional<T>> : basic_optional_parser<api::optional<T>> {};

template <typename T>
struct boost_domless_parser<box<T>> : basic_optional_parser<box<T>> {};

}  // namespace tgbm::generator_parser
