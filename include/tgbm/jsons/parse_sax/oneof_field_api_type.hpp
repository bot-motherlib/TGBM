#pragma once

#include <bitset>

#include <utility>

#include "tgbm/jsons/sax.hpp"

#include "tgbm/tools/api_utils.hpp"
#include "tgbm/tools/traits.hpp"

namespace tgbm::json {

template <oneof_field_api_type T>
struct sax_parser<T> {
  static constexpr auto N = pfr_extension::tuple_size_v<T> - 1;

  static_assert(std::same_as<pfr_extension::tuple_element_t<N, T>, decltype(T::data)>);

  static sax_consumer_t get_generator_field(T& t_, std::string_view key, sax_token& tok,
                                            std::bitset<N>& parsed_) {
    return pfr_extension::visit_struct_field<T, sax_consumer_t, N>(
        key,
        [&]<size_t I>() {
          using Field = pfr_extension::tuple_element_t<I, T>;
          auto& field = pfr_extension::get<I>(t_);
          if (parsed_.test(I))
            TGBM_JSON_PARSE_ERROR;
          parsed_.set(I);
          return sax_parser<Field>::parse(field, tok);
        },
        // unknown field (discriminated 'data' or unknown field which must be ignored)
        [&]() {
          return oneof_field_utils::emplace_field<T, sax_consumer_t>(
              t_.data, key,
              [&]<typename Field>(Field& field) { return sax_parser<Field>::parse(field, tok); },
              []() -> sax_consumer_t { TGBM_JSON_PARSE_ERROR; },
              [&]() -> sax_consumer_t { return sax_ignore_value(tok); });
        });
  }

  static sax_consumer_t parse(T& v, sax_token& tok) {
    using enum sax_token::kind_e;
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
}  // namespace tgbm::json
