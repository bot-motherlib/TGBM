#pragma once

#include <kelcoro/stack_memory_resource.hpp>

#include <tgbm/jsons/sax.hpp>
#include <tgbm/utils/traits.hpp>

namespace tgbm::json {

template <discriminated_api_type T>
struct sax_parser<T> {
  static sax_consumer_t get_generator_suboneof(std::string_view key, T& v, sax_token& tok,
                                               dd::with_stack_resource r) {
    auto emplacer = [&]<typename Suboneof>() -> sax_consumer_t {
      if constexpr (!std::same_as<Suboneof, void>)
        return sax_parser<Suboneof>::parse(v.data.template emplace<Suboneof>(), tok, r);
      else
        return sax_ignore_value(tok);
    };
    return v.discriminate(key, emplacer);
  }

  static sax_consumer_t bufferized_parse(T& out, sax_token& tok, dd::with_stack_resource r) {
    using enum sax_token::kind_e;

    assert(tok.got != key || tok.str_m != T::discriminator);

    std::vector<sax_token> buf_toks;
    buf_toks.reserve(10);
    {
      sax_token token;
      token.got = object_begin;
      buf_toks.emplace_back(token);
    }

    size_t depth = 1;
    do {
      switch (tok.got) {
        case sax_token::array_begin:
        case sax_token::object_begin:
          ++depth;
          break;
        case sax_token::array_end:
        case sax_token::object_end:
          --depth;
          [[fallthrough]];
        default:
          break;
      }
      buf_toks.emplace_back(tok);
      co_yield {};
    } while (!(depth == 1 && tok.got == key && tok.str_m == T::discriminator));

    co_yield {};
    tok.expect(string);

    sax_consumer_t gen_suboneof = get_generator_suboneof(tok.str_m, out, tok, r);
    auto it = gen_suboneof.cur_iterator();
    for (auto& buf_tok : buf_toks) {
      if (it == gen_suboneof.end())
        throw parse_error("oneof parsing reached end while tokens are not");
      tok = buf_tok;
      ++it;
    }

    if (it != gen_suboneof.end()) {
      co_yield {};
      co_yield dd::elements_of(gen_suboneof);
    }
  }

  static sax_consumer_t parse(T& out, sax_token& tok, dd::with_stack_resource r) {
    using enum sax_token::kind_e;
    tok.expect(object_begin);
    co_yield {};
    if (tok.got == object_end)
      co_return;
    tok.expect(key);
    if (tok.got != key || tok.str_m != T::discriminator) [[unlikely]] {
      co_yield dd::elements_of(bufferized_parse(out, tok, r));
      co_return;
    }
    co_yield {};
    tok.expect(string);
    // change 'got' before generator creation (may be function returning generator)
    tok.got = object_begin;
    co_yield dd::elements_of(get_generator_suboneof(tok.str_m, out, tok, r));
  }
};

}  // namespace tgbm::json
