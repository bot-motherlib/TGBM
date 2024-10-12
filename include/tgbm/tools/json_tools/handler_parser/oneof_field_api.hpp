#pragma once
#include <bitset>
#include <cassert>
#include <optional>
#include <utility>
#include <boost/pfr/core.hpp>
#include <tgbm/tools/json_tools/handler_parser/basic_parser.hpp>
#include <tgbm/tools/json_tools/handler_parser/ignore.hpp>
#include <tgbm/tools/api_utils.hpp>
#include <tgbm/tools/pfr_extension.hpp>
#include <tgbm/tools/traits.hpp>
#include <tgbm/tools/StringTools.h>

namespace tgbm::json::handler_parser {

template <oneof_field_api_type T>
struct parser<T> : basic_parser<T> {
  using basic_parser<T>::basic_parser;
  static constexpr auto N = oneof_field_utils::N<T>;

  std::bitset<N> parsed_fields_{};
  bool started_{};

  ResultParse start_object() {
    assert(!this->parsed_);
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
    assert(!this->parsed_);
    TGBM_ON_DEBUG({ this->parsed_ = true; });
    if (all_parsed()) {
      return ResultParse::kEnd;
    } else
      TGBM_JSON_HANDLER_PARSE_ERROR;
  }

  ResultParse emplace_parser(std::string_view key) {
    return pfr_extension::visit_struct_field<T, ResultParse, N>(
        key,
        [&]<std::size_t I> {
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
    assert(!this->parsed_);
    return emplace_parser(val);
  }
};

}  // namespace tgbm::json::handler_parser
