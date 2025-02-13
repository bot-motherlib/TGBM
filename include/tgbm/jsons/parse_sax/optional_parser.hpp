#pragma once

#include <kelcoro/stack_memory_resource.hpp>

#include <tgbm/jsons/sax.hpp>
#include <tgbm/api/optional.hpp>
#include <tgbm/utils/box.hpp>

namespace tgbm::json {

template <typename Optional>
struct basic_optional_parser {
  using T = typename Optional::value_type;

  TGBM_GCC_WORKAROUND static sax_consumer_t parse(Optional& v, sax_token& tok, dd::with_stack_resource r) {
    if (tok.got == sax_token::null) [[unlikely]]
      return {};
    return sax_parser<T>::parse(v.emplace(), tok, r);
  }
};

template <typename T>
struct sax_parser<std::optional<T>> : basic_optional_parser<std::optional<T>> {};

template <typename T>
struct sax_parser<api::optional<T>> : basic_optional_parser<api::optional<T>> {};

template <typename T>
struct sax_parser<box<T>> : basic_optional_parser<box<T>> {};

}  // namespace tgbm::json
