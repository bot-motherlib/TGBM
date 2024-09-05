#pragma once

#include <cassert>
#include <concepts>
#include <string_view>
#include <tgbm/tools/pfr_extension.hpp>
#include <tgbm/logger.h>
#include <tgbm/tools/macro.hpp>
#include <anyany/anyany.hpp>
#include <boost/container/static_vector.hpp>
#include <fmt/ranges.h>
#include <tgbm/tools/json_tools/exceptions.hpp>

namespace tgbm::json::handler_parser {

template <typename T>
struct Parser {};

#ifndef TGBM_STATIC_PARSER_SIZE
#define TGBM_STATIC_PARSER_SIZE 64
#endif

#ifndef TGBM_PARSER_STACK_SIZE
#define TGBM_PARSER_STACK_SIZE 16
#endif

enum struct ResultParse { kContinue, kError, kEnd };

inline ResultParse err() {
  return ResultParse::kError;
}
#define TGBM_JSON_HANDLER_PARSE_ERROR return err()
// TGBM_JSON_HANDLER_PARSE_ERROR

namespace details {
struct StartObject {
  static ResultParse do_invoke(auto& self) {
    return self.start_object();
  }

  template <typename CRTP>
  struct plugin {
    ResultParse start_object() {
      auto& self = *static_cast<CRTP*>(this);
      return aa::invoke<StartObject>(self);
    }
  };
};
struct EndObject {
  static ResultParse do_invoke(auto& self) {
    return self.end_object();
  }

  template <typename CRTP>
  struct plugin {
    ResultParse end_object() {
      auto& self = *static_cast<CRTP*>(this);
      return aa::invoke<EndObject>(self);
    }
  };
};

struct StartArray {
  static ResultParse do_invoke(auto& self) {
    return self.start_array();
  }

  template <typename CRTP>
  struct plugin {
    ResultParse start_array() {
      auto& self = *static_cast<CRTP*>(this);
      return aa::invoke<StartArray>(self);
    }
  };
};

struct EndArray {
  static ResultParse do_invoke(auto& self) {
    return self.end_array();
  }

  template <typename CRTP>
  struct plugin {
    ResultParse end_array() {
      auto& self = *static_cast<CRTP*>(this);
      return aa::invoke<EndArray>(self);
    }
  };
};

struct Key {
  static ResultParse do_invoke(auto& self, std::string_view val) {
    return self.key(val);
  }

  template <typename CRTP>
  struct plugin {
    ResultParse key(std::string_view val) {
      auto& self = *static_cast<CRTP*>(this);
      return aa::invoke<Key>(self, val);
    }
  };
};

struct String {
  static ResultParse do_invoke(auto& self, std::string_view val) {
    return self.string(val);
  }

  template <typename CRTP>
  struct plugin {
    ResultParse string(std::string_view val) {
      auto& self = *static_cast<CRTP*>(this);
      return aa::invoke<String>(self, val);
    }
  };
};

struct Integral {
  static ResultParse do_invoke(auto& self, std::int64_t val) {
    return self.integral(val);
  }

  template <typename CRTP>
  struct plugin {
    ResultParse integral(std::int64_t val) {
      auto& self = *static_cast<CRTP*>(this);
      return aa::invoke<Integral>(self, val);
    }
  };
};

struct UnsignedIntegral {
  static ResultParse do_invoke(auto& self, std::uint64_t val) {
    return self.unsigned_integral(val);
  }

  template <typename CRTP>
  struct plugin {
    ResultParse unsigned_integral(std::uint64_t val) {
      auto& self = *static_cast<CRTP*>(this);
      return aa::invoke<UnsignedIntegral>(self, val);
    }
  };
};

struct Floating {
  static ResultParse do_invoke(auto& self, double val) {
    return self.floating(val);
  }

  template <typename CRTP>
  struct plugin {
    ResultParse floating(double val) {
      auto& self = *static_cast<CRTP*>(this);
      return aa::invoke<Floating>(self, val);
    }
  };
};

struct Boolean {
  static ResultParse do_invoke(auto& self, bool val) {
    return self.boolean(val);
  }

  template <typename CRTP>
  struct plugin {
    ResultParse boolean(bool val) {
      auto& self = *static_cast<CRTP*>(this);
      return aa::invoke<Boolean>(self, val);
    }
  };
};

struct Null {
  static ResultParse do_invoke(auto& self) {
    return self.null();
  }

  template <typename CRTP>
  struct plugin {
    ResultParse null() {
      auto& self = *static_cast<CRTP*>(this);
      return aa::invoke<Null>(self);
    }
  };
};

struct OnNextEndParsing {
  static ResultParse do_invoke(auto& self) {
    return self.on_next_end_parsing();
  }

  template <typename CRTP>
  struct plugin {
    ResultParse on_next_parsing() {
      auto& self = *static_cast<CRTP*>(this);
      return aa::invoke<OnNextEndParsing>(self);
    }
  };
};

#ifdef TGBM_ENABLE_DEBUG

struct NameType {
  template <typename T>
  static std::string_view do_invoke(const T& self) {
    return aa::noexport::n<T>();
  }

  template <typename CRTP>
  struct plugin {
    std::string_view name_type() {
      auto& self = *static_cast<CRTP*>(this);
      return aa::invoke<NameType>(self);
    }
  };
};

template <std::size_t Size>
using ErasedParser = aa::basic_any_with<aa::unreachable_allocator, Size, NameType, aa::move, StartObject,
                                        EndObject, StartArray, EndArray, Key, String, Integral,
                                        UnsignedIntegral, Floating, Boolean, Null, OnNextEndParsing>;
#else
template <std::size_t Size>
using ErasedParser = aa::basic_any_with<aa::unreachable_allocator, Size, aa::move, StartObject, EndObject,
                                        StartArray, EndArray, Key, String, Integral, UnsignedIntegral,
                                        Floating, Boolean, Null, OnNextEndParsing>;
#endif
}  // namespace details

using ErasedParser = details::ErasedParser<TGBM_STATIC_PARSER_SIZE>;

struct ParserStack {
  ::boost::container::static_vector<ErasedParser, TGBM_STATIC_PARSER_SIZE> stack_;
  TGBM_DEBUG_FIELD(stack_types_, std::vector<std::string_view>);

 public:
  void push(ErasedParser parser) {
    if (stack_types_.size() == stack_types_.max_size()) {
      TGBM_JSON_PARSE_ERROR;
    }
    TGBM_ON_DEBUG({ stack_types_.push_back(parser.name_type()); });
    stack_.push_back(std::move(parser));
  }

  void pop() {
    TGBM_ON_DEBUG({ stack_types_.pop_back(); });
    assert(!stack_.empty());
    stack_.pop_back();
  }

  bool empty() {
    return stack_.empty();
  }

  ErasedParser& last() {
    assert(!stack_.empty());

    return stack_.back();
  }

  ResultParse handle_result(ResultParse res) {
    while (res == ResultParse::kEnd && !stack_.empty()) [[unlikely]] {
      pop();
      if (!stack_.empty()) {
        res = on_next_end_parsing();
      }
    }
    return res;
  }

  ResultParse on_next_end_parsing() {
    return last().on_next_parsing();
  }

  bool start_object() {
    auto res = last().start_object();
    res = handle_result(res);
    return res != ResultParse::kError;
  }
  bool end_object() {
    auto res = last().end_object();
    res = handle_result(res);
    return res != ResultParse::kError;
  }
  bool start_array() {
    auto res = last().start_array();
    res = handle_result(res);
    return res != ResultParse::kError;
  }
  bool end_array() {
    auto res = last().end_array();
    res = handle_result(res);
    return res != ResultParse::kError;
  }
  bool key(std::string_view val) {
    auto res = last().key(val);
    res = handle_result(res);
    return res != ResultParse::kError;
  }
  bool string(std::string_view val) {
    auto res = last().string(val);
    res = handle_result(res);
    return res != ResultParse::kError;
  }
  bool integral(std::int64_t val) {
    auto res = last().integral(val);
    res = handle_result(res);
    return res != ResultParse::kError;
  }
  bool unsigned_integral(std::uint64_t val) {
    auto res = last().unsigned_integral(val);
    res = handle_result(res);
    return res != ResultParse::kError;
  }
  bool floating(double val) {
    auto res = last().floating(val);
    res = handle_result(res);
    return res != ResultParse::kError;
  }
  bool boolean(bool val) {
    auto res = last().boolean(val);
    res = handle_result(res);
    return res != ResultParse::kError;
  }
  bool null() {
    auto res = last().null();
    res = handle_result(res);
    return res != ResultParse::kError;
  }
};

template <typename T>
struct basic_parser {
  ParserStack& parser_stack_;
  T* t_ = nullptr;

  TGBM_DEBUG_FIELD(parsed_, bool);

  basic_parser(ParserStack& stack, T& t) : parser_stack_(stack), t_(std::addressof(t)) {
    parsed_ = false;
  }

  basic_parser(ParserStack& stack, T* t = nullptr) : parser_stack_(stack), t_(t) {
    parsed_ = false;
  }

  ResultParse on_next_end_parsing() {
    return ResultParse::kContinue;
  }

  ResultParse start_object() {
    TGBM_JSON_HANDLER_PARSE_ERROR;
  }
  ResultParse end_object() {
    TGBM_JSON_HANDLER_PARSE_ERROR;
  }
  ResultParse start_array() {
    TGBM_JSON_HANDLER_PARSE_ERROR;
  }
  ResultParse end_array() {
    TGBM_JSON_HANDLER_PARSE_ERROR;
  }
  ResultParse key(std::string_view val) {
    TGBM_JSON_HANDLER_PARSE_ERROR;
  }
  ResultParse string(std::string_view val) {
    TGBM_JSON_HANDLER_PARSE_ERROR;
  }
  ResultParse integral(std::int64_t val) {
    TGBM_JSON_HANDLER_PARSE_ERROR;
  }
  ResultParse unsigned_integral(std::uint64_t val) {
    TGBM_JSON_HANDLER_PARSE_ERROR;
  }
  ResultParse floating(double val) {
    TGBM_JSON_HANDLER_PARSE_ERROR;
  }
  ResultParse boolean(bool val) {
    TGBM_JSON_HANDLER_PARSE_ERROR;
  }
  ResultParse null() {
    TGBM_JSON_HANDLER_PARSE_ERROR;
  }
};

template <>
struct basic_parser<void> {
  basic_parser() = default;
  void on_start_parsing() {
  }

  ResultParse start_object() {
    TGBM_JSON_HANDLER_PARSE_ERROR;
  }
  ResultParse end_object() {
    TGBM_JSON_HANDLER_PARSE_ERROR;
  }
  ResultParse start_array() {
    TGBM_JSON_HANDLER_PARSE_ERROR;
  }
  ResultParse end_array() {
    TGBM_JSON_HANDLER_PARSE_ERROR;
  }
  ResultParse key(std::string_view val) {
    TGBM_JSON_HANDLER_PARSE_ERROR;
  }
  ResultParse string(std::string_view val) {
    TGBM_JSON_HANDLER_PARSE_ERROR;
  }
  ResultParse integral(std::int64_t val) {
    TGBM_JSON_HANDLER_PARSE_ERROR;
  }
  ResultParse unsigned_integral(std::uint64_t val) {
    TGBM_JSON_HANDLER_PARSE_ERROR;
  }
  ResultParse floating(double val) {
    TGBM_JSON_HANDLER_PARSE_ERROR;
  }
  ResultParse boolean(bool val) {
    TGBM_JSON_HANDLER_PARSE_ERROR;
  }
  ResultParse null() {
    TGBM_JSON_HANDLER_PARSE_ERROR;
  }
};

template <typename T>
struct parser {
  parser<T>(ParserStack& stack, T& t) {
    static_assert(!std::same_as<T, T>);
  }
};

}  // namespace tgbm::json::handler_parser
