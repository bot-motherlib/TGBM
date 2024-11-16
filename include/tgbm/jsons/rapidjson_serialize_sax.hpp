#pragma once

#include <cstdint>
#include <variant>
#include <cassert>

#include <rapidjson/writer.h>

#include "tgbm/api/Integer.hpp"
#include "tgbm/api/arrayof.hpp"
#include "tgbm/api/const_string.hpp"
#include "tgbm/api/true.hpp"
#include "tgbm/utils/traits.hpp"
#include "tgbm/api/oneof.hpp"
#include "tgbm/utils/pfr_extension.hpp"
#include "tgbm/api/optional.hpp"
#include "tgbm/api/floating.hpp"
#include "tgbm/utils/box.hpp"
#include "tgbm/api/file_or_str.hpp"

namespace tgbm {

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

static void rj_assume(bool b) {
  assert(b);
  (void)b;
}

void rj_tojson(rjson_writer auto& writer, const array_like auto& rng) {
  rj_assume(writer.StartArray());
  for (auto&& item : rng)
    rj_tojson(writer, item);
  rj_assume(writer.EndArray());
}

void rj_tojson(rjson_writer auto& writer, bool b) {
  rj_assume(writer.Bool(b));
}

void rj_tojson(rjson_writer auto& writer, string_like auto&& v) {
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

template <typename... Args>
void rj_tojson(rjson_writer auto& writer, const std::variant<Args...>& v) {
  std::visit([&](const auto& x) { rj_tojson(writer, x); }, v);
}

template <typename... Args>
void rj_tojson(rjson_writer auto& writer, const box_union<Args...>& v) {
  v.visit(matcher{[](tgbm::nothing_t) {}, [&](auto& x) { rj_tojson(writer, x); }});
}

namespace noexport {

template <typename T>
[[nodiscard]] const T* optional_field_get_value(const api::optional<T>& f) noexcept {
  return f ? f.operator->() : nullptr;
}
template <typename T>
[[nodiscard]] const T* optional_field_get_value(const box<T>& f) noexcept {
  return f ? f.operator->() : nullptr;
}
template <typename... Args>
[[nodiscard]] const auto* optional_field_get_value(const api::oneof<Args...>& f) noexcept {
  return f ? &f : nullptr;
}
template <typename T>
[[nodiscard]] const auto* optional_field_get_value(const api::arrayof<T>& f) noexcept {
  return !f.empty() ? &f : nullptr;
}

}  // namespace noexport

template <common_api_type A>
void rj_tojson(rjson_writer auto& writer, const A& v) {
  pfr_extension::visit_object(v, [&]<typename Info>(auto& field) {
    constexpr std::string_view name = Info::name.AsStringView();
    if constexpr (!A::is_mandatory_field(name)) {
      auto* f = noexport::optional_field_get_value(field);
      if (!f)
        return;
      rj_assume(writer.Key(name.data(), name.size()));
      rj_tojson(writer, *f);
    } else {
      rj_assume(writer.Key(name.data(), name.size()));
      rj_tojson(writer, field);
    }
  });
}

template <oneof_field_api_type A>
void rj_tojson(rjson_writer auto& writer, const A& v) {
  static_assert(boost::pfr::tuple_size_v<A> == 2, "now only first mandatory field + oneof .data supported");
  constexpr std::string_view name = boost::pfr::get_name<0, A>();

  rj_assume(writer.Key(name.data(), name.size()));
  rj_tojson(writer, boost::pfr::get<0>(v));
  if (!v.data)
    return;
  std::string_view d = v.discriminator_now();
  rj_assume(writer.Key(d.data(), d.size()));
  v.data.visit(matcher{[](tgbm::nothing_t) { unreachable(); }, [&](auto& x) { rj_tojson(writer, x.value); }});
}

template <discriminated_api_type A>
void rj_tojson(rjson_writer auto& writer, const A& v) {
  if (!v.data)
    return;
  std::string_view d = v.discriminator_now();
  rj_assume(writer.Key(d.data(), d.size()));
  v.data.visit(matcher{[](tgbm::nothing_t) { unreachable(); }, [&](auto& x) { rj_tojson(writer, x); }});
}

void rj_tojson(rjson_writer auto& writer, const const_string& v) {
  rj_tojson(writer, v.str());
}

void rj_tojson(rjson_writer auto& writer, const api::Integer& v) {
  rj_tojson(writer, v.value);
}

void rj_tojson(rjson_writer auto& writer, const api::Double& v) {
  rj_tojson(writer, v.value);
}

void rj_tojson(rjson_writer auto& writer, api::True) {
  rj_tojson(writer, true);
}

void rj_tojson(rjson_writer auto& writer, const api::file_or_str& f) {
  assert(!f.is_file());
  if (auto* s = f.get_str())
    rj_tojson(writer, *s);
}

template <typename T>
void rj_tojson(rjson_writer auto& writer, const box<T>& v) {
  if (!v) [[unlikely]] {
    rj_assume(writer.Null());
    return;
  }
  rj_tojson(writer, *v);
}

// require explicit cast to int
void rj_tojson(rjson_writer auto& writer, char_type_like auto) = delete;

}  // namespace tgbm
