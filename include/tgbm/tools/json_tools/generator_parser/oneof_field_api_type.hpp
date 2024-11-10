#pragma once

#include <bitset>

#include <utility>

#include "tgbm/tools/json_tools/generator_parser/basic_parser.hpp"
#include "tgbm/tools/json_tools/generator_parser/ignore_parser.hpp"

#include "tgbm/tools/api_utils.hpp"
#include "tgbm/tools/traits.hpp"

namespace tgbm::generator_parser {

template <oneof_field_api_type T>
struct boost_domless_parser<T> {
  static constexpr auto N = pfr_extension::tuple_size_v<T> - 1;

  static_assert(std::same_as<pfr_extension::tuple_element_t<N, T>, decltype(T::data)>);

  static dd::generator<nothing_t> get_generator_field(T& t_, std::string_view key, event_holder& tok,
                                                      std::bitset<N>& parsed_, memres_tag auto resource) {
    return pfr_extension::visit_struct_field<T, dd::generator<nothing_t>, N>(
        key,
        [&]<std::size_t I>() {
          using Field = pfr_extension::tuple_element_t<I, T>;
          auto& field = pfr_extension::get<I>(t_);
          using parser = boost_domless_parser<Field>;
          if (parsed_.test(I))
            TGBM_JSON_PARSE_ERROR;
          parsed_.set(I);
          return parser::parse(field, tok, std::move(resource));
        },
        // unknown field (discriminated 'data' or unknown field which must be ignored)
        [&]() {
          return oneof_field_utils::emplace_field<T, dd::generator<nothing_t>>(
              t_.data, key,
              [&]<typename Field>(Field& field) {
                return boost_domless_parser<Field>::parse(field, tok, std::move(resource));
              },
              []() -> dd::generator<nothing_t> { TGBM_JSON_PARSE_ERROR; },
              [&]() -> dd::generator<nothing_t> { return ignore_parser::parse(tok, std::move(resource)); });
        });
  }

  static dd::generator<nothing_t> parse(T& v, event_holder& tok, memres_tag auto resource) {
    std::bitset<N> parsed_;

    tok.expect(tok.object_begin);

    for (;;) {
      co_yield {};
      if (tok.got == tok.object_end)
        break;
      tok.expect(tok.key);
      std::string_view key = tok.str_m;
      co_yield {};
      co_yield dd::elements_of(get_generator_field(v, key, tok, parsed_, resource));
    }
    if (!parsed_.all())
      TGBM_JSON_PARSE_ERROR;
    assert(tok.got == tok.object_end);
  }
};
}  // namespace tgbm::generator_parser
