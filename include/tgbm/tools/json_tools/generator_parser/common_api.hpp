#pragma once
#include <bitset>
#include <tgbm/tools/json_tools/generator_parser/basic_parser.hpp>
#include "tgbm/tools/json_tools/generator_parser/ignore_parser.hpp"
#include "tgbm/tools/pfr_extension.hpp"
#include "tgbm/tools/traits.hpp"

namespace tgbm::generator_parser {

template <common_api_type T>
struct boost_domless_parser<T> {
  static constexpr auto N = ::pfr_extension::tuple_size_v<T>;
  static constexpr bool simple = false;
  using seq = std::make_index_sequence<N>;

  static bool all_parsed(std::bitset<N>& parsed_) {
    static std::bitset<N> required_mask = []<std::size_t... I>(std::index_sequence<I...>) {
      std::bitset<N> res;
      auto store_required = [&]<std::size_t J>(std::index_sequence<J>) {
        constexpr std::string_view name = pfr_extension::element_name_v<J, T>;
        res[J] = !T::is_optional_field(name);
      };
      (store_required(std::index_sequence<I>{}), ...);
      return res;
    }(seq{});
    return (parsed_ & required_mask) == required_mask;
  }

  static dd::generator<nothing_t> generator_field(T& t_, std::string_view key, event_holder& holder,
                                                  std::bitset<N>& parsed_) {
    if constexpr (N > 0) {
      return pfr_extension::visit_struct_field<T, dd::generator<nothing_t>>(
          key,
          [&]<std::size_t I>() {
            using Field = pfr_extension::tuple_element_t<I, T>;
            auto& field = pfr_extension::get<I>(t_);
            using parser = boost_domless_parser<Field>;
            if (parsed_[I]) {
              TGBM_JSON_PARSE_ERROR;
            }
            parsed_[I] = true;
            if constexpr (is_simple<Field>) {
              parser::simple_parse(field, holder);
              return dd::generator<nothing_t>{};
            } else {
              return parser::parse(field, holder);
            }
          },
          [&]() { return ignore_parser::parse(holder); });
    } else {
      return ignore_parser::parse(holder);
    }
  }

  static dd::generator<nothing_t> parse(T& t_, event_holder& holder) {
    using wait = event_holder::wait_e;
    std::bitset<N> parsed_;
    holder.expect(wait::object_begin);

    while (true) {
      co_yield {};
      holder.expect(wait::key | wait::object_end);
      if (holder.got == wait::object_end) {
        break;
      }
      std::string_view cur_key = holder.str_m;
      co_yield {};
      auto gen_field = generator_field(t_, cur_key, holder, parsed_);
      if (!gen_field.empty()) {
        co_yield (dd::elements_of(gen_field));
      }
    }
    if (!all_parsed(parsed_)) {
      TGBM_JSON_PARSE_ERROR;
    }
  }
};
}  // namespace tgbm::generator_parser
