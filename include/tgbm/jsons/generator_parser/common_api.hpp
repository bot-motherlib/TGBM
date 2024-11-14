#pragma once

#include <bitset>

#include "tgbm/jsons/generator_parser/basic_parser.hpp"
#include "tgbm/jsons/generator_parser/ignore_parser.hpp"
#include "tgbm/tools/pfr_extension.hpp"
#include "tgbm/tools/traits.hpp"

namespace tgbm::json::sax {

template <common_api_type T>
struct parser<T> {
  static constexpr auto N = ::pfr_extension::tuple_size_v<T>;

  static bool all_parsed(const std::bitset<N>& parsed_) {
    static std::bitset<N> required_mask = []<std::size_t... I>(std::index_sequence<I...>) {
      std::bitset<N> res;
      auto store_required = [&]<std::size_t J>(std::index_sequence<J>) {
        constexpr std::string_view name = pfr_extension::element_name_v<J, T>;
        res[J] = T::is_mandatory_field(name);
      };
      (store_required(std::index_sequence<I>{}), ...);
      return res;
    }(std::make_index_sequence<N>{});
    return (parsed_ & required_mask) == required_mask;
  }

  static parser_t generator_field(T& v, std::string_view key, event_holder& tok, std::bitset<N>& parsed_) {
    if constexpr (N > 0) {
      return pfr_extension::visit_struct_field<T, parser_t>(
          key,
          [&]<std::size_t I>() {
            auto& field = pfr_extension::get<I>(v);
            if (parsed_.test(I))
              TGBM_JSON_PARSE_ERROR;
            parsed_.set(I);
            return parser<pfr_extension::tuple_element_t<I, T>>::parse(field, tok);
          },
          [&]() { return ignore_parser::parse(tok); });
    } else {
      return ignore_parser::parse(tok);
    }
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
      std::string_view cur_key = tok.str_m;
      co_yield {};
      co_yield dd::elements_of(generator_field(v, cur_key, tok, parsed_));
    }
    if (!all_parsed(parsed_)) [[unlikely]]
      TGBM_JSON_PARSE_ERROR;
  }
};

}  // namespace tgbm::json::sax
