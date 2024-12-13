#pragma once

#include <cassert>
#include <utility>
#include <string_view>

#include <tgbm/utils/pfr_extension.hpp>
#include "basic_parser.hpp"
#include <tgbm/utils/traits.hpp>

namespace tgbm::json::handler_parser {

template <discriminated_api_type T>
struct parser<T> : basic_parser<T> {
  using basic_parser<T>::basic_parser;

  using Data = decltype(T::data);
  static constexpr auto N = pfr_extension::tuple_size_v<T>;

  using seq = std::make_index_sequence<N>;

  bool started_ = false;

  ResultParse start_object() {
    if (!started_) {
      started_ = true;
      return ResultParse::kContinue;
    }
    TGBM_JSON_HANDLER_PARSE_ERROR;
  }

  ResultParse end_object() {
    return ResultParse::kEnd;
  }

  ResultParse on_next_end_parsing() {
    return ResultParse::kEnd;
  }

  ResultParse emplace_parser(std::string_view key) {
    auto emplacer = [&]<typename Suboneof>() -> ResultParse {
      if constexpr (!std::same_as<Suboneof, void>) {
        auto& suboneof = this->t_->data.template emplace<Suboneof>();
        this->parser_stack_.push(parser<Suboneof>{this->parser_stack_, suboneof});
        this->parser_stack_.start_object();
        return ResultParse::kContinue;
      } else {
        TGBM_JSON_HANDLER_PARSE_ERROR;
      }
    };
    return this->t_->discriminate(key, emplacer);
  }

  ResultParse key(std::string_view val) {
    if (val != T::discriminator) {
      TGBM_JSON_HANDLER_PARSE_ERROR;
    }
    return ResultParse::kContinue;
  }

  ResultParse string(std::string_view val) {
    return emplace_parser(val);
  }
};

}  // namespace tgbm::json::handler_parser
