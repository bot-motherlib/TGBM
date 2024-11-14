#pragma once

#include "tgbm/jsons/generator_parser/basic_parser.hpp"
#include "tgbm/api/optional.hpp"
#include "tgbm/tools/box.hpp"

namespace tgbm::json::sax {

template <typename Optional>
struct basic_optional_parser {
  using T = typename Optional::value_type;

  static parser_t parse(Optional& v, event_holder& tok) {
    if (tok.got == event_holder::null) [[unlikely]]
      return {};
    return parser<T>::parse(v.emplace(), tok);
  }
};

template <typename T>
struct parser<std::optional<T>> : basic_optional_parser<std::optional<T>> {};

template <typename T>
struct parser<api::optional<T>> : basic_optional_parser<api::optional<T>> {};

template <typename T>
struct parser<box<T>> : basic_optional_parser<box<T>> {};

}  // namespace tgbm::json::sax
