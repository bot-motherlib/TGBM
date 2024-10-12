#pragma once
#include <bitset>
#include <tgbm/tools/json_tools/generator_parser/basic_parser.hpp>
#include <utility>
#include "boost/pfr/core.hpp"
#include "tgbm/tools/api_utils.hpp"
#include "tgbm/tools/json_tools/generator_parser/ignore_parser.hpp"
#include "tgbm/tools/traits.hpp"

namespace tgbm::generator_parser {

template <oneof_field_api_type T>
struct boost_domless_parser<T> {
  static constexpr auto N = ::boost::pfr::tuple_size_v<T> - 1;
  using seq = std::make_index_sequence<N>;
  static constexpr bool simple = false;

  // проверяем, что oneof лежит последним полем data
  static_assert(std::same_as<::boost::pfr::tuple_element_t<N, T>, decltype(T::data)>);

  static bool all_parsed(std::bitset<N>& parsed_) {
    return parsed_.all();
  }

  static generator get_generator_field(T& t_, std::string_view key, event_holder& holder,
                                                      std::bitset<N>& parsed_) {
    generator result;
    auto opt_gen = pfr_extension::visit_struct_field<T, bool, N>(
        key,
        [&]<std::size_t I>() {
          using Field = ::boost::pfr::tuple_element_t<I, T>;
          auto& field = ::boost::pfr::get<I>(t_);
          using parser = boost_domless_parser<Field>;
          if (parsed_[I]) {
            TGBM_JSON_PARSE_ERROR;
          }
          parsed_[I] = true;
          if constexpr (is_simple<Field>) {
            parser::simple_parse(field, holder);
            result = generator{};
            return true;
          } else {
            result = parser::parse(field, holder);
            return true;
          }
        },
        [&]() { return false; });
    if (opt_gen) {
      return result;
    }
    result = oneof_field_utils::emplace_field<T, generator>(
        t_.data, key,
        [&]<typename Field>(Field& field) {
          using parser = boost_domless_parser<Field>;
          if constexpr (!is_simple<Field>) {
            return boost_domless_parser<Field>::parse(field, holder);
          } else {
            boost_domless_parser<Field>::simple_parse(field, holder);
            return generator{};
          }
        },
        []() -> generator { TGBM_JSON_PARSE_ERROR; },
        []() -> generator { return {}; });
    return result;
  }

  static generator parse(T& t_, event_holder& holder) {
    using wait = event_holder::wait_e;
    std::bitset<N> parsed_;

    holder.expect(wait::object_begin);

    while (true) {
      co_yield {};
      holder.expect(wait::key | wait::object_end);
      if (holder.got == wait::object_end) {
        break;
      }
      std::string_view key = holder.str_m;
      co_yield {};
      auto generator_suboneof = get_generator_field(t_, key, holder, parsed_);
      co_yield dd::elements_of(generator_suboneof);
    }

    if (!all_parsed(parsed_)) {
      TGBM_JSON_PARSE_ERROR;
    }
    assert(holder.got == event_holder::object_end);
  }
};
}  // namespace tgbm::generator_parser
