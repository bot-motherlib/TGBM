#pragma once

#if 0

#include <tgbm/tools/json_parser/ignore.h>
#include <bitset>
#include <cassert>
#include <optional>
#include <type_traits>
#include <utility>
#include "boost/pfr/core.hpp"
#include <tgbm/tools/StringTools.h>

namespace tgbm::json {
template <typename T>
  requires std::is_aggregate_v<T>
class tuple_parsers {
  using seq = std::make_index_sequence<boost::pfr::tuple_size_v<T>>;

  template <std::size_t J>
  using parser_field = parser<boost::pfr::tuple_element_t<J, T>>;

  template <std::size_t J>
  using pointer_field = std::add_pointer_t<boost::pfr::tuple_element_t<J, T>>;

  template <std::size_t... I>
  static auto type_helper(std::integer_sequence<std::size_t, I...> seq) {
    return std::type_identity<std::tuple<parser_field<I>...>>{};
  }

 public:
  using type = decltype(type_helper(seq{}))::type;

 private:
  template <std::size_t... I>
  static auto make_helper(T& t, std::integer_sequence<std::size_t, I...> seq) {
    return type{boost::pfr::get<I>(t)...};
  }
  template <std::size_t... I>
  static auto make_helper(T* t, std::integer_sequence<std::size_t, I...> seq) {
    if (t) {
      return make_helper(*t, seq);
    }
    return type{parser_field<I>{(pointer_field<I>)nullptr}...};
  }

 public:
  static auto make(T& t) {
    return make_helper(t, seq{});
  }
  static auto make(T* t) {
    return make_helper(t, seq{});
  }
};

template <typename T>
using tuple_parsers_t = tuple_parsers<T>::type;

template <typename T>
  requires std::is_aggregate_v<T>
struct parser<T> : basic_parser<T> {
  static constexpr auto N = boost::pfr::tuple_size_v<T>;

  tuple_parsers_t<T> parsers_;
  ignore_parser ignore_parser{};
  std::bitset<N> parsed_fields_{};
  std::size_t cur_index_ = empty;
  bool started_{};

  enum indexes : std::size_t { empty = std::size_t(-1), unknown = std::size_t(-2) };

  using seq = std::make_index_sequence<N>;

  parser(T& t) : basic_parser<T>(t), parsers_(tuple_parsers<T>::make(t)) {
  }
  parser(T* t) : basic_parser<T>(t), parsers_(tuple_parsers<T>::make(t)) {
  }

  void rebind(T* t) {
    this->t_ = t;
    parsers_ = tuple_parsers<T>::make(t);
    parsed_fields_ = {};
    cur_index_ = empty;
    started_ = {};
  }

  ResultParse start_object() {
    assert(!this->parsed_);
    if (!started_) {
      started_ = true;
      return ResultParse::kContinue;
    }
    auto visiter = [&](auto& parser) { return parser.start_object(); };
    return get_result_parse(visiter);
  }

  ResultParse handle_field_result(ResultParse result) {
    if (result == ResultParse::kEnd) {
      parsed_fields_[cur_index_] = true;
      cur_index_ = empty;
      return ResultParse::kContinue;
    } else if (result == ResultParse::kContinue) {
      return ResultParse::kContinue;
    } else {
      JSON_PARSE_ERROR;
    }
  }

  template <std::size_t... J>
  bool check_parsed(std::index_sequence<J...>) {
    bool result = false;
    auto helper = [&]<std::size_t I>() {
      using type = boost::pfr::tuple_element_t<I, T>;
      return !std::get<I>(parsers_).required || parsed_fields_[I];
    };
    return (helper.template operator()<J>() && ...);
  }

  ResultParse end_object() {
    assert(!this->parsed_);
    if (cur_index_ == empty) {
      this->parsed_ = true;
      if (check_parsed(seq{})) {
        return ResultParse::kEnd;
      } else
        JSON_PARSE_ERROR;
    }
    return get_result_parse([&](auto& parser) { return parser.end_object(); });
  }

  ResultParse start_array() {
    assert(!this->parsed_);
    return get_result_parse([&](auto& parser) { return parser.start_array(); });
  }

  ResultParse end_array() {
    assert(!this->parsed_);
    return get_result_parse([&](auto& parser) { return parser.end_array(); });
  }

  ResultParse key(std::string_view val) {
    assert(!this->parsed_);
    if (cur_index_ == empty) {
      cur_index_ = count_index(val);
      visit_parser([&](auto& parser) { parser.on_start_parsing(); }, seq{});
      return ResultParse::kContinue;
    } else {
      return get_result_parse([&](auto& parser) { return parser.key(val); });
    }
  }

  ResultParse string(std::string_view val) {
    assert(!this->parsed_);
    return get_result_parse([&](auto& parser) { return parser.string(val); });
  }

  ResultParse integral(std::int64_t val) {
    assert(!this->parsed_);
    return get_result_parse([&](auto& parser) { return parser.integral(val); });
  }

  ResultParse unsigned_integral(std::uint64_t val) {
    assert(!this->parsed_);
    return get_result_parse([&](auto& parser) { return parser.unsigned_integral(val); });
  }

  ResultParse floating(double val) {
    assert(!this->parsed_);
    return get_result_parse([&](auto& parser) { return parser.floating(val); });
  }

  ResultParse boolean(bool val) {
    assert(!this->parsed_);
    return get_result_parse([&](auto& parser) { return parser.boolean(val); });
  }

  ResultParse null() {
    assert(!this->parsed_);
    return get_result_parse([&](auto& parser) { return parser.null(); });
  }

  template <std::size_t... I>
  static constexpr std::size_t count_index_impl(std::string_view key,
                                                std::integer_sequence<std::size_t, I...>) {
    using switcher = utils::string_switch<std::size_t>;
    return (switcher{key} | ... | switcher::case_t(boost::pfr::get_name<I, T>(), I)).or_default(unknown);
  }

  static constexpr std::size_t count_index(std::string_view key) {
    return count_index_impl(key, seq{});
  }

  template <std::size_t... I>
  void visit_parser(auto&& callable, std::integer_sequence<std::size_t, I...>) {
    assert(cur_index_ != empty);
    if (cur_index_ == unknown) {
      callable(ignore_parser);
    }
    auto helper = [&]<std::size_t J>() {
      if (cur_index_ != J) {
        return false;
      } else {
        callable(std::get<J>(parsers_));
        return true;
      }
    };
    (helper.template operator()<I>() || ...);
  }

  ResultParse get_result_parse(auto&& callable) {
    if (cur_index_ == empty) {
      JSON_PARSE_ERROR;
    }
    std::optional<ResultParse> result_parse;
    auto visiter = [&](auto& parser) { result_parse = callable(parser); };
    visit_parser(visiter, seq{});
    assert(result_parse.has_value());
    return handle_field_result(*result_parse);
  }
};

}  // namespace tgbm::json

#endif
