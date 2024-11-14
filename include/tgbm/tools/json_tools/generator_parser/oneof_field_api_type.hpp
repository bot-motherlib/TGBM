#pragma once

#include <bitset>

#include <utility>

#include "tgbm/tools/json_tools/generator_parser/basic_parser.hpp"
#include "tgbm/tools/json_tools/generator_parser/ignore_parser.hpp"

#include "tgbm/tools/api_utils.hpp"
#include "tgbm/tools/traits.hpp"

namespace tgbm::json::sax {

template <oneof_field_api_type T>
struct parser<T> {
  static constexpr auto N = pfr_extension::tuple_size_v<T> - 1;

  static_assert(std::same_as<pfr_extension::tuple_element_t<N, T>, decltype(T::data)>);

  static parser_t get_generator_field(T& t_, std::string_view key, event_holder& tok,
                                      std::bitset<N>& parsed_) {
    return pfr_extension::visit_struct_field<T, parser_t, N>(
        key,
        [&]<std::size_t I>() {
          using Field = pfr_extension::tuple_element_t<I, T>;
          auto& field = pfr_extension::get<I>(t_);
          if (parsed_.test(I))
            TGBM_JSON_PARSE_ERROR;
          parsed_.set(I);
          return parser<Field>::parse(field, tok);
        },
        // unknown field (discriminated 'data' or unknown field which must be ignored)
        [&]() {
          return oneof_field_utils::emplace_field<T, parser_t>(
              t_.data, key, [&]<typename Field>(Field& field) { return parser<Field>::parse(field, tok); },
              []() -> parser_t { TGBM_JSON_PARSE_ERROR; },
              [&]() -> parser_t { return ignore_parser::parse(tok); });
        });
  }

  static parser_t parse(T& v, event_holder& tok) {
    using enum event_holder::wait_e;
    std::bitset<N> parsed_;

    tok.expect(object_begin);

    for (;;) {
      co_yield {};
      if (tok.got == object_end)
        break;
      tok.expect(key);
      std::string_view key = tok.str_m;
      co_yield {};
      co_yield dd::elements_of(get_generator_field(v, key, tok, parsed_));
    }
    if (!parsed_.all())
      TGBM_JSON_PARSE_ERROR;
    assert(tok.got == object_end);
  }
};
}  // namespace tgbm::json::sax
