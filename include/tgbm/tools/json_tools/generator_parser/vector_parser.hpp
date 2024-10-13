#pragma once
#include <tgbm/tools/json_tools/generator_parser/basic_parser.hpp>

namespace tgbm::generator_parser {

template <typename T>
struct boost_domless_parser<std::vector<T>> {
  static constexpr bool simple = false;

  static generator parse(std::vector<T>& t_, tgbm::generator_parser::event_holder& holder, with_pmr r) {
    holder.expect(event_holder::array_begin);
    while (true) {
      co_yield {};
      if (holder.got == event_holder::array_end) {
        break;
      }
      auto& cur = t_.emplace_back();
      if constexpr (is_simple<T>) {
        boost_domless_parser<T>::simple_parse(cur, holder);
      } else {
        co_yield dd::elements_of(boost_domless_parser<T>::parse(cur, holder, r));
      }
    }
  }
};
}  // namespace tgbm::generator_parser
