#pragma once

#include "tgbm/jsons/sax.hpp"
#include "tgbm/tools/traits.hpp"

namespace tgbm::json {

template <discriminated_api_type T>
struct sax_parser<T> {
  static sax_consumer_t get_generator_suboneof(std::string_view key, T& v, sax_token& tok) {
    auto emplacer = [&]<typename Suboneof>() -> sax_consumer_t {
      if constexpr (!std::same_as<Suboneof, void>)
        return sax_parser<Suboneof>::parse(v.data.template emplace<Suboneof>(), tok);
      else
        TGBM_JSON_PARSE_ERROR;
    };
    return v.discriminate(key, emplacer);
  }

  static sax_consumer_t parse(T& v, sax_token& tok) {
    using enum sax_token::kind_e;
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

}  // namespace tgbm::json
