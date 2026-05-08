#pragma once

#include <cstring>
#include <tgbm/utils/pfr_extension.hpp>
#include <tgbm/jsons/errors.hpp>
#include <format>

#include <kelcoro/generator.hpp>

namespace tgbm::json {

struct sax_token {
  enum kind_e : uint16_t {
    array_begin = 1 << 0,
    array_end = 1 << 1,
    object_begin = 1 << 2,
    object_end = 1 << 3,
    string = 1 << 4,
    key = 1 << 5,
    int64 = 1 << 6,
    uint64 = 1 << 7,
    double_ = 1 << 8,
    bool_ = 1 << 9,
    null = 1 << 10,
    part = 1 << 11,
  };

  union {
    double double_m;
    int64_t int_m;
    uint64_t uint_m;
    std::string_view str_m;
    bool bool_m;
  };

  kind_e got = sax_token::object_begin;

  sax_token() {
  }
  ~sax_token() {
  }

  void expect(uint16_t tokens) const {
    if (!(got & tokens))
      throw parse_error(
          std::format("unexpected token bits `{:b}`, expected bits: `{:b}`", uint16_t(got), tokens));
  }
};

struct sax_token_value {
  union {
    double double_m;
    int64_t int_m;
    uint64_t uint_m;
    std::string str_m;
    bool bool_m;
  };

  sax_token_value(sax_token token) {
    using enum sax_token::kind_e;
    got = token.got;
    switch (got) {
      case string:
      case key:
      case part:
        std::construct_at(&str_m, token.str_m);
        [[fallthrough]];
      case array_begin:
      case array_end:
      case object_begin:
      case object_end:
      case null:
        return;
      case int64:
        int_m = token.int_m;
        return;
      case uint64:
        uint_m = token.uint_m;
        return;
      case double_:
        double_m = token.double_m;
        return;
      case bool_:
        bool_m = token.bool_m;
        return;
    }
  }

  sax_token_value() {
  }
  sax_token_value(const sax_token_value&) = delete;

  sax_token_value(sax_token_value&& rhs) noexcept {
    if (is_string(rhs.got)) {
      got = rhs.got;
      std::construct_at(&str_m, std::move(rhs.str_m));
    } else {
      std::memcpy((void*)this, &rhs, sizeof(*this));
    }
  }

  sax_token_value& operator=(sax_token_value&& rhs) noexcept {
    assert(&rhs != this);  // for perf
    std::destroy_at(this);
    std::construct_at(this, std::move(rhs));
    return *this;
  }

  sax_token to_view() const noexcept {
    using enum sax_token::kind_e;
    sax_token token;
    token.got = got;
    switch (got) {
      case string:
      case key:
      case part:
        token.str_m = str_m;
        [[fallthrough]];
      case array_begin:
      case array_end:
      case object_begin:
      case object_end:
      case null:
        break;
      case int64:
        token.int_m = int_m;
        break;
      case uint64:
        token.uint_m = uint_m;
        break;
      case double_:
        token.double_m = double_m;
        break;
      case bool_:
        token.bool_m = bool_m;
        break;
    }

    return token;
  }

  static bool is_string(sax_token::kind_e got) noexcept {
    using enum sax_token::kind_e;
    switch (got) {
      case string:
      case key:
      case part:
        return true;
      case array_begin:
      case array_end:
      case object_begin:
      case object_end:
      case int64:
      case uint64:
      case double_:
      case bool_:
      case null:
        return false;
    }
    unreachable();
  }

  ~sax_token_value() {
    destroy();
  }

  sax_token::kind_e got = sax_token::object_begin;

  void expect(uint16_t tokens) const {
    if (!(got & tokens))
      throw parse_error(
          std::format("unexpected token bits `{:b}`, expected bits: `{:b}`", uint16_t(got), tokens));
  }

 private:
  void destroy() noexcept {
    if (is_string(got)) {
      std::destroy_at(&str_m);
      got = sax_token::object_begin;
    }
  }
};

using sax_consumer_t = dd::generator<dd::nothing_t>;

template <typename>
struct sax_parser {
  // required interface:
  //   static sax_consumer_t parse(T& v, sax_token& tok)
  // Note: 'tok' never will be 'part', its handled by token producer
};

inline sax_consumer_t sax_ignore_value(sax_token& tok) {
  switch (tok.got) {
    case sax_token::string:
    case sax_token::int64:
    case sax_token::uint64:
    case sax_token::double_:
    case sax_token::bool_:
    case sax_token::null:
      co_return;
    default:
      break;
  }
  size_t depth = 0;
  for (;;) {
    switch (tok.got) {
      case sax_token::array_begin:
      case sax_token::object_begin:
        depth++;
        co_yield {};
        break;
      case sax_token::part:
        unreachable();
      case sax_token::array_end:
      case sax_token::object_end:
        depth--;
        if (depth == 0)
          co_return;
        [[fallthrough]];
      case sax_token::string:
      case sax_token::int64:
      case sax_token::uint64:
      case sax_token::double_:
      case sax_token::bool_:
      case sax_token::null:
      case sax_token::key:
        co_yield {};
    }
  }
}

}  // namespace tgbm::json

namespace std {

template <>
struct formatter<::tgbm::json::sax_token> : std::formatter<std::string_view> {
  auto format(tgbm::json::sax_token const& tok, auto& ctx) const -> decltype(ctx.out()) {
    switch (tok.got) {
      case tgbm::json::sax_token::array_begin:
        return std::format_to(ctx.out(), "array_begin");
      case tgbm::json::sax_token::array_end:
        return std::format_to(ctx.out(), "array_end");
      case tgbm::json::sax_token::object_begin:
        return std::format_to(ctx.out(), "object_begin");
      case tgbm::json::sax_token::object_end:
        return std::format_to(ctx.out(), "object_end");
      case tgbm::json::sax_token::string:
        return std::format_to(ctx.out(), "string `{}`", tok.str_m);
      case tgbm::json::sax_token::key:
        return std::format_to(ctx.out(), "key `{}`", tok.str_m);
      case tgbm::json::sax_token::int64:
        return std::format_to(ctx.out(), "int64 `{}`", tok.int_m);
      case tgbm::json::sax_token::uint64:
        return std::format_to(ctx.out(), "uint64 `{}`", tok.uint_m);
      case tgbm::json::sax_token::double_:
        return std::format_to(ctx.out(), "double `{}`", tok.double_m);
      case tgbm::json::sax_token::bool_:
        return std::format_to(ctx.out(), "boolean `{}`", tok.bool_m);
      case tgbm::json::sax_token::null:
        return std::format_to(ctx.out(), "null");
      case tgbm::json::sax_token::part:
        return std::format_to(ctx.out(), "part `{}`", tok.str_m);
    }
    tgbm::unreachable();
  }
};

}  // namespace std
