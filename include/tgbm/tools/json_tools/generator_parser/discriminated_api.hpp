#pragma once

#include "tgbm/tools/json_tools/generator_parser/basic_parser.hpp"
#include "tgbm/tools/traits.hpp"

namespace tgbm::json::sax {

template <discriminated_api_type T>
struct parser<T> {
  static parser_t get_generator_suboneof(std::string_view key, T& v, event_holder& tok) {
    auto emplacer = [&]<typename Suboneof>() -> parser_t {
      if constexpr (!std::same_as<Suboneof, void>)
        return parser<Suboneof>::parse(v.data.template emplace<Suboneof>(), tok);
      else
        TGBM_JSON_PARSE_ERROR;
    };
    return v.discriminate(key, emplacer);
  }

  static parser_t parse(T& v, event_holder& tok) {
    using enum event_holder::wait_e;
    tok.expect(object_begin);
    co_yield {};
    if (tok.got == object_end)
      co_return;
    tok.expect(key);
    if (tok.got != key || tok.str_m != T::discriminator) [[unlikely]]
      json::throw_json_parse_error();
    co_yield {};
    tok.expect(string);
    // change 'got' before generator creation (may be function returning generator)
    tok.got = object_begin;
    co_yield dd::elements_of(get_generator_suboneof(tok.str_m, v, tok));
  }
};

}  // namespace tgbm::json::sax
