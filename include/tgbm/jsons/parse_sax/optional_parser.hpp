#pragma once

#include "tgbm/jsons/sax.hpp"
#include "tgbm/api/optional.hpp"
#include "tgbm/utils/box.hpp"

namespace tgbm::json {

template <typename Optional>
struct basic_optional_parser {
  using T = typename Optional::value_type;

  static sax_consumer_t parse(Optional& v, sax_token& tok) {
    if (tok.got == sax_token::null) [[unlikely]]
      return {};
    return sax_parser<T>::parse(v.emplace(), tok);
  }
};

template <typename T>
struct sax_parser<std::optional<T>> : basic_optional_parser<std::optional<T>> {};

template <typename T>
struct sax_parser<api::optional<T>> : basic_optional_parser<api::optional<T>> {};

template <typename T>
struct sax_parser<box<T>> : basic_optional_parser<box<T>> {};

}  // namespace tgbm::json
