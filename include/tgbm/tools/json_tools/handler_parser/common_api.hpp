#pragma once
#include <bitset>
#include <cassert>
#include <optional>
#include <utility>
#include <tgbm/tools/json_tools/handler_parser/basic_parser.hpp>
#include <tgbm/tools/json_tools/handler_parser/ignore.hpp>
#include "tgbm/tools/traits.hpp"
#include <tgbm/tools/StringTools.h>

namespace tgbm::json::handler_parser {

template <common_api_type T>
struct parser<T> : basic_parser<T> {
  using basic_parser<T>::basic_parser;
  static constexpr auto N = pfr_extension::tuple_size_v<T>;

  enum indexes : std::size_t { unknown = N, empty = N + 1 };

  std::bitset<N> parsed_fields_{};
  bool started_{};

  using seq = std::make_index_sequence<N>;

  ResultParse start_object() {
    assert(!this->parsed_);
    if (!started_) {
      started_ = true;
      return ResultParse::kContinue;
    }
    TGBM_JSON_HANDLER_PARSE_ERROR;
  }

  static constexpr std::array<bool, N> is_optional = []<std::size_t... I>(std::index_sequence<I...>) {
    auto helper = []<std::size_t J>(std::index_sequence<J>) {
      constexpr std::string_view name = pfr_extension::get_name<J, T>();
      constexpr std::optional<bool> val = T::is_optional_field(name);
      static_assert(val.has_value());
      return *val;
    };
    return std::array<bool, N>{helper(std::index_sequence<I>{})...};
  }(seq{});

  bool all_parsed() {
    for (std::size_t i = 0; i < N; i++) {
      if (!is_optional[i] && !parsed_fields_[i]) {
        return false;
      }
    }
    return true;
  }

  ResultParse end_object() {
    assert(!this->parsed_);
    TGBM_ON_DEBUG({ this->parsed_ = true; });
    if (all_parsed()) {
      return ResultParse::kEnd;
    } else
      TGBM_JSON_HANDLER_PARSE_ERROR;
  }

  void emplace_parser(std::size_t index) {
    auto helper = [&]<std::size_t I>() {
      if constexpr (I == unknown) {
        this->parser_stack_.push(ignore_parser{});
        return true;
      } else {
        if (I == index) {
          static_assert(I < pfr_extension::tuple_size_v<T>);
          parsed_fields_[I] = true;
          using Parser = parser<pfr_extension::tuple_element_t<I, T>>;
          auto& field = pfr_extension::get<I>(*this->t_);
          Parser parser{this->parser_stack_, field};
          this->parser_stack_.push(std::move(parser));
          return true;
        } else {
          return false;
        }
      }
      return false;
    };
    auto val = [&]<std::size_t... I>(std::index_sequence<I...>) {
      return (helper.template operator()<I>() || ...);
    }(std::make_index_sequence<N + 1>{});
    assert(val);
  }

  ResultParse key(std::string_view val) {
    assert(!this->parsed_);
    auto index = count_index(val);
    emplace_parser(index);

    return ResultParse::kContinue;
  }

  template <std::size_t... I>
  static constexpr std::size_t count_index_impl(std::string_view key,
                                                std::integer_sequence<std::size_t, I...>) {
    using switcher = utils::string_switch<std::size_t>;
    return (switcher{key} | ... | switcher::case_t(pfr_extension::get_name<I, T>(), I)).or_default(unknown);
  }

  static constexpr std::size_t count_index(std::string_view key) {
    return count_index_impl(key, seq{});
  }
};

}  // namespace tgbm::json::handler_parser
