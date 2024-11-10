#pragma once

#include "tgbm/tools/json_tools/generator_parser/basic_parser.hpp"
#include "tgbm/tools/traits.hpp"

namespace tgbm::generator_parser {

template <discriminated_api_type T>
struct boost_domless_parser<T> {
  static dd::generator<nothing_t> get_generator_suboneof(std::string_view key, T& v, event_holder& tok,
                                                         memres_tag auto resource) {
    auto emplacer = [&]<typename Suboneof>() -> dd::generator<nothing_t> {
      if constexpr (!std::same_as<Suboneof, void>)
        return boost_domless_parser<Suboneof>::parse(v.data.template emplace<Suboneof>(), tok,
                                                     std::move(resource));
      else
        TGBM_JSON_PARSE_ERROR;
    };
    return v.discriminate(key, emplacer);
  }

  static dd::generator<nothing_t> parse(T& v, event_holder& tok, memres_tag auto resource) {
    tok.expect(tok.object_begin);
    co_yield {};
    if (tok.got == tok.object_end)
      co_return;
    tok.expect(tok.key);
    if (tok.got != tok.key || tok.str_m != T::discriminator) [[unlikely]]
      json::throw_json_parse_error();
    co_yield {};
    tok.expect(tok.string);
    // change 'got' before generator creation (may be function returning generator)
    tok.got = event_holder::object_begin;
    co_yield dd::elements_of(get_generator_suboneof(tok.str_m, v, tok, resource));
  }
};

}  // namespace tgbm::generator_parser
