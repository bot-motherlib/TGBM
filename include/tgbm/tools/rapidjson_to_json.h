#pragma once

#include <cstdint>
#include <rapidjson/writer.h>

namespace tgbm {

template <typename T, typename... Types>
concept any_of = (std::is_same_v<T, Types> || ...);

template <typename T>
concept char_type_like = any_of<T, char, signed char, unsigned char, char8_t, char16_t, char32_t, wchar_t>;

template <typename T>
concept numeric = std::is_integral_v<T> && !char_type_like<T> && !std::is_same_v<T, bool>;

template <typename T>
concept rjson_writer = requires(T& w) {
  w.Null();
  w.Bool(true);
  w.Int(5);
  w.Uint(5u);
  w.Int64(int64_t(5));
  w.Uint64(uint64_t(5));
  w.Double(5.);
  w.String((const char*)0, 0);

  w.StartObject();
  w.EndObject();

  w.Key((const char*)0, 0);
  w.StartArray();

  w.EndArray();
};

static_assert(!rjson_writer<int>);
static_assert(rjson_writer<rapidjson::Writer<int>>);

[[gnu::always_inline]] static void rj_assume(bool b) {
  assert(b);
  (void)b;
}

template <std::ranges::range R>
void rj_tojson(rjson_writer auto& writer, const R& rng)
  requires(!std::convertible_to<R, std::string_view>)
{
  rj_assume(writer.StartArray());
  for (auto&& item : rng)
    rj_tojson(writer, item);
  rj_assume(writer.EndArray());
}

void rj_tojson(rjson_writer auto& writer, bool b) {
  rj_assume(writer.Bool(b));
}

void rj_tojson(rjson_writer auto& writer, std::convertible_to<std::string_view> auto&& v) {
  std::string_view str = v;
  rj_assume(writer.String(str.data(), str.size()));
}

void rj_tojson(rjson_writer auto& writer, numeric auto numb) {
  using T = std::remove_cvref_t<decltype(numb)>;
  if constexpr (std::is_unsigned_v<T>) {
    if constexpr (sizeof(T) > sizeof(int32_t)) {
      rj_assume(writer.Uint64(numb));
    } else {
      rj_assume(writer.Uint(numb));
    }
  } else {
    if constexpr (sizeof(T) > sizeof(int32_t)) {
      rj_assume(writer.Int64(numb));
    } else {
      rj_assume(writer.Int(numb));
    }
  }
}

void rj_tojson(rjson_writer auto& writer, std::floating_point auto f) {
  // rapid json very bad handle doubles, but ok
  rj_assume(writer.Double(f));
}

void rj_tojson(rjson_writer auto& writer, char_type_like auto) = delete;

}  // namespace tgbm
