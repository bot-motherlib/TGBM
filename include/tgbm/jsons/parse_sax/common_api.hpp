#pragma once

#include <bitset>

#include "tgbm/jsons/sax.hpp"
#include "tgbm/utils/pfr_extension.hpp"
#include "tgbm/utils/traits.hpp"

namespace tgbm::json {

template <common_api_type T>
struct sax_parser<T> {
  static constexpr auto N = ::pfr_extension::tuple_size_v<T>;

  static bool all_parsed(const std::bitset<N>& parsed_) {
    static std::bitset<N> required_mask = []<size_t... I>(std::index_sequence<I...>) {
      std::bitset<N> res;
      auto store_required = [&]<size_t J>(std::index_sequence<J>) {
        constexpr std::string_view name = pfr_extension::element_name_v<J, T>;
        res[J] = T::is_mandatory_field(name);
      };
      (store_required(std::index_sequence<I>{}), ...);
      return res;
    }(std::make_index_sequence<N>{});
    return (parsed_ & required_mask) == required_mask;
  }

  static sax_consumer_t generator_field(T& v, std::string_view key, sax_token& tok, std::bitset<N>& parsed_) {
    if constexpr (N > 0) {
      return pfr_extension::visit_struct_field<T, sax_consumer_t>(
          key,
          [&]<size_t I>() {
            auto& field = pfr_extension::get<I>(v);
            if (parsed_.test(I))
              TGBM_JSON_PARSE_ERROR;
            parsed_.set(I);
            return sax_parser<pfr_extension::tuple_element_t<I, T>>::parse(field, tok);
          },
          [&]() { return sax_ignore_value(tok); });
    } else {
      return sax_ignore_value(tok);
    }
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
      std::string_view cur_key = tok.str_m;
      co_yield {};
      co_yield dd::elements_of(generator_field(v, cur_key, tok, parsed_));
    }
    if (!all_parsed(parsed_)) [[unlikely]]
      TGBM_JSON_PARSE_ERROR;
  }
};

}  // namespace tgbm::json
