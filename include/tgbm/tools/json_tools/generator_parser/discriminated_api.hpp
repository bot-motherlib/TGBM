#pragma once

#include "tgbm/tools/json_tools/generator_parser/basic_parser.hpp"
#include "tgbm/tools/traits.hpp"

namespace tgbm::generator_parser {

template <discriminated_api_type T>
struct boost_domless_parser<T> {
  static constexpr auto N = T::variant_size;
  static constexpr bool simple = false;

  static dd::generator<nothing_t> get_generator_suboneof(std::string_view key, T& t_, event_holder& holder) {
    auto emplacer = [&]<typename Suboneof>() -> dd::generator<nothing_t> {
      if constexpr (!std::same_as<Suboneof, void>) {
        auto& suboneof = t_.data.template emplace<Suboneof>();
        return boost_domless_parser<Suboneof>::parse(suboneof, holder);
      } else {
        TGBM_JSON_PARSE_ERROR;
      }
    };
    return t_.discriminate(key, emplacer);
  }

  static dd::generator<nothing_t> parse(T& t_, event_holder& holder) {
    using wait = event_holder::wait_e;

    holder.expect(wait::object_begin);

    co_yield {};
    holder.expect(wait::key | wait::object_end);
    if (holder.got == wait::object_end) {
      co_return;
    }
    co_yield {};
    holder.expect(wait::string);
    // change 'got' before generator creation (may be function returning generator)
    holder.got = event_holder::object_begin;
    auto generator_suboneof = get_generator_suboneof(holder.str_m, t_, holder);
    auto it = generator_suboneof.begin();
    co_yield {};
    co_yield dd::elements_of(generator_suboneof);
    assert(holder.got == event_holder::object_end);
  }
};
}  // namespace tgbm::generator_parser
