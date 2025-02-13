#pragma once

#include <bitset>

#include <kelcoro/stack_memory_resource.hpp>

#include <tgbm/jsons/sax.hpp>
#include <tgbm/utils/pfr_extension.hpp>
#include <tgbm/utils/traits.hpp>
#include <tgbm/logger.hpp>

namespace tgbm::json {

namespace noexport {

template <typename T, size_t N, size_t... Is>
TGBM_GCC_WORKAROUND inline std::bitset<::pfr_extension::tuple_size_v<T>> make_required_mask(
    std::index_sequence<Is...>) {
  std::bitset<N> res;
  (res.set(Is, T::is_mandatory_field(pfr_extension::element_name_v<Is, T>)), ...);
  return res;
}

template <typename T>
const inline std::bitset<::pfr_extension::tuple_size_v<T>> required_mask =
    make_required_mask<T, ::pfr_extension::tuple_size_v<T>>(
        std::make_index_sequence<::pfr_extension::tuple_size_v<T>>{});

}  // namespace noexport

template <common_api_type T>
struct sax_parser<T> {
  static constexpr auto N = ::pfr_extension::tuple_size_v<T>;

  static bool all_parsed(const std::bitset<N>& parsed_) {
    return (parsed_ & noexport::required_mask<T>) == noexport::required_mask<T>;
  }

  static sax_consumer_t generator_field(T& v, std::string_view key, sax_token& tok, std::bitset<N>& parsed_,
                                        dd::with_stack_resource r) {
    if constexpr (N > 0) {
      return pfr_extension::visit_struct_field<T, sax_consumer_t>(
          key,
          [&]<size_t I>() {
            auto& field = pfr_extension::get<I>(v);
            if (parsed_.test(I))
              TGBM_JSON_PARSE_ERROR;
            parsed_.set(I);
            return sax_parser<pfr_extension::tuple_element_t<I, T>>::parse(field, tok, r);
          },
          [&]() { return sax_ignore_value(tok); });
    } else {
      return sax_ignore_value(tok);
    }
  }

  static sax_consumer_t parse(T& v, sax_token& tok, dd::with_stack_resource r) {
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
      co_yield dd::elements_of(generator_field(v, cur_key, tok, parsed_, r));
    }
    if (!all_parsed(parsed_)) [[unlikely]]
      TGBM_JSON_PARSE_ERROR;
  }
};

}  // namespace tgbm::json
