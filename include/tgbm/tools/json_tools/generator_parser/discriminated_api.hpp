#pragma once
#include <tgbm/tools/json_tools/generator_parser/basic_parser.hpp>
#include "tgbm/tools/json_tools/generator_parser/ignore_parser.hpp"
#include "tgbm/tools/traits.hpp"

namespace tgbm::generator_parser {

template <discriminated_api_type T>
struct boost_domless_parser<T> {
  static constexpr auto N = T::variant_size;
  static constexpr bool simple = false;

  static generator get_generator_suboneof(std::string_view key, T& t_, event_holder& holder, with_pmr r) {
    auto emplacer = [&]<typename Suboneof>() -> generator {
      if constexpr (!std::same_as<Suboneof, void>) {
        auto& suboneof = t_.data.template emplace<Suboneof>();
        return boost_domless_parser<Suboneof>::parse(suboneof, holder, r);
      } else {
        TGBM_JSON_PARSE_ERROR;
      }
    };
    return t_.discriminate(key, emplacer);
  }

  static generator parse(T& t_, event_holder& holder, with_pmr r) {
    using wait = event_holder::wait_e;

    holder.expect(wait::object_begin);

    co_yield {};
    holder.expect(wait::key | wait::object_end);
    if (holder.got == wait::object_end) {
      co_return;
    }
    co_yield {};
    holder.expect(wait::string);
    auto generator_suboneof = get_generator_suboneof(holder.str_m, t_, holder, r);
    holder.got = event_holder::object_begin;
    auto it = generator_suboneof.begin();
    co_yield {};
    co_yield dd::elements_of(generator_suboneof);
    assert(holder.got == event_holder::object_end);
  }
};
}  // namespace tgbm::generator_parser
