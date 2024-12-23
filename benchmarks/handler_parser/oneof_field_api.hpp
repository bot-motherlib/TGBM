#pragma once

#include <bitset>
#include <cassert>
#include <utility>
#include <string_view>

#include <tgbm/utils/pfr_extension.hpp>
#include "basic_parser.hpp"
#include "ignore.hpp"
#include <tgbm/utils/api_utils.hpp>
#include <tgbm/utils/pfr_extension.hpp>
#include <tgbm/utils/traits.hpp>

namespace tgbm::json::handler_parser {

template <oneof_field_api_type T>
struct parser<T> : basic_parser<T> {
  using basic_parser<T>::basic_parser;
  static constexpr auto N = oneof_field_utils::N<T>;

  std::bitset<N> parsed_fields_{};
  bool started_{};

  ResultParse start_object() {
    if (!started_) {
      started_ = true;
      return ResultParse::kContinue;
    }
    TGBM_JSON_HANDLER_PARSE_ERROR;
  }

  bool all_parsed() {
    return parsed_fields_.all();
  }

  ResultParse end_object() {
    if (all_parsed()) {
      return ResultParse::kEnd;
    } else
      TGBM_JSON_HANDLER_PARSE_ERROR;
  }

  ResultParse emplace_parser(std::string_view key) {
    return pfr_extension::visit_struct_field<T, ResultParse, N>(
        key,
        [&]<size_t I> {
          using Field = pfr_extension::tuple_element_t<I, T>;
          using Parser = parser<Field>;
          auto& field = pfr_extension::get<I>(*this->t_);
          if (parsed_fields_[I]) {
            TGBM_JSON_HANDLER_PARSE_ERROR;
          }
          parsed_fields_[I] = true;
          Parser parser{this->parser_stack_, field};
          this->parser_stack_.push(std::move(parser));
          return ResultParse::kContinue;
        },
        [&] {
          return oneof_field_utils::emplace_field<T, ResultParse>(
              this->t_->data, key,
              [&]<typename Field>(Field& field) {
                using Parser = parser<Field>;
                this->parser_stack_.push(Parser{this->parser_stack_, field});
                return ResultParse::kContinue;
              },
              [] { TGBM_JSON_HANDLER_PARSE_ERROR; },
              [&] {
                this->parser_stack_.push(ignore_parser{});
                return ResultParse::kContinue;
              });
        });
  }

  ResultParse key(std::string_view val) {
    return emplace_parser(val);
  }
};

}  // namespace tgbm::json::handler_parser
