#pragma once

#include "tgbm/tools/json_tools/generator_parser/basic_parser.hpp"
#include "tgbm/api/optional.hpp"
#include "tgbm/tools/box.hpp"

namespace tgbm::generator_parser {

template <typename Optional>
struct basic_optional_parser {
  using T = typename Optional::value_type;
  static constexpr bool simple = is_simple<T>;
  static dd::generator<nothing_t> parse(Optional& t_, event_holder& holder) {
    if (holder.got == event_holder::null) {
      co_return;
    }
    using parser = boost_domless_parser<T>;
    co_yield dd::elements_of(parser::parse(t_.emplace(), holder));
  }
  static void simple_parse(Optional& t_, event_holder& holder)
    requires(simple)
  {
    if (holder.got == event_holder::null) {
      return;
    }
    using parser = boost_domless_parser<T>;
    parser::simple_parse(t_.emplace(), holder);
  }
};

template <typename T>
struct boost_domless_parser<std::optional<T>> : basic_optional_parser<std::optional<T>> {};

template <typename T>
struct boost_domless_parser<api::optional<T>> : basic_optional_parser<api::optional<T>> {};

template <typename T>
struct boost_domless_parser<box<T>> : basic_optional_parser<box<T>> {};

}  // namespace tgbm::generator_parser
