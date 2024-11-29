#pragma once

#include <cstdint>
#include <variant>
#include <cassert>

#include <rapidjson/writer.h>

#include <tgbm/api/Integer.hpp>
#include <tgbm/api/arrayof.hpp>
#include <tgbm/api/const_string.hpp>
#include <tgbm/api/true.hpp>
#include <tgbm/utils/traits.hpp>
#include <tgbm/api/oneof.hpp>
#include <tgbm/utils/pfr_extension.hpp>
#include <tgbm/api/optional.hpp>
#include <tgbm/api/floating.hpp>
#include <tgbm/utils/box.hpp>
#include <tgbm/api/file_or_str.hpp>
#include "tgbm/api/int_or_str.hpp"

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

// required interface: static void serialize(W&, const T&);
template <rjson_writer W, typename T>
struct rj_tojson;

template <rjson_writer W, array_like A>
struct rj_tojson<W, A> {
  static void serialize(W& writer, const A& rng) {
    (void)writer.StartArray();
    for (auto&& item : rng)
      rj_tojson<W, std::ranges::range_value_t<A>>::serialize(writer, item);
    (void)writer.EndArray();
  }
};

template <rjson_writer W>
struct rj_tojson<W, bool> {
  static void serialize(W& writer, bool b) {
    (void)writer.Bool(b);
  }
};

template <rjson_writer W, string_like S>
struct rj_tojson<W, S> {
  static void serialize(W& writer, const S& v) {
    std::string_view str(v);
    (void)writer.String(str.data(), str.size());
  }
};

template <rjson_writer W, numeric Num>
struct rj_tojson<W, Num> {
  static void serialize(W& writer, Num numb) {
    using T = std::remove_cvref_t<decltype(numb)>;
    if constexpr (std::is_unsigned_v<T>) {
      if constexpr (sizeof(T) > sizeof(int32_t)) {
        (void)writer.Uint64(numb);
      } else {
        (void)writer.Uint(numb);
      }
    } else {
      if constexpr (sizeof(T) > sizeof(int32_t)) {
        (void)writer.Int64(numb);
      } else {
        (void)writer.Int(numb);
      }
    }
  }
};

template <rjson_writer W, std::floating_point Num>
struct rj_tojson<W, Num> {
  static void serialize(W& writer, Num f) {
    // rapid json very bad handle doubles, but ok
    (void)writer.Double(f);
  }
};

template <rjson_writer W>
struct rj_tojson<W, api::int_or_str> {
  static void serialize(W& writer, const api::int_or_str& v) {
    std::visit([&]<typename T>(const T& x) { rj_tojson<W, T>::serialize(writer, x); }, v);
  }
};

template <rjson_writer W, typename... Args>
struct rj_tojson<W, box_union<Args...>> {
  static void serialize(W& writer, const box_union<Args...>& v) {
    v.visit(matcher{[](tgbm::nothing_t) {},
                    [&]<typename T>(const T& x) { rj_tojson<W, T>::serialize(writer, x); }});
  }
};

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

}  // namespace noexport

template <rjson_writer W>
struct rj_tojson<W, const_string> {
  static void serialize(W& writer, const const_string& v) {
    rj_tojson<W, std::string_view>(writer, v.str());
  }
};
// TODO fn serialize + remove cv ref

template <rjson_writer W>
struct rj_tojson<W, api::Integer> {
  static void serialize(W& writer, const api::Integer& v) {
    rj_tojson<W, decltype(v.value)>::serialize(writer, v.value);
  }
};

template <rjson_writer W>
struct rj_tojson<W, api::Double> {
  static void serialize(W& writer, const api::Double& v) {
    rj_tojson<W, double>::serialize(writer, v.value);
  }
};

template <rjson_writer W>
struct rj_tojson<W, api::True> {
  static void serialize(W& writer, api::True) {
    rj_tojson<W, bool>::serialize(writer, true);
  }
};

template <rjson_writer W>
struct rj_tojson<W, api::file_or_str> {
  static void serialize(W& writer, const api::file_or_str& v) {
    // must not be valueless since no there are throwing move ctors
    assert(!v.data.valueless_by_exception());
    if (std::holds_alternative<std::monostate>(v.data)) [[unlikely]]
      writer.Null();
    else if (auto* f = v.get_file()) {
      writer.String(f->filename.data(), f->filename.size());
      return;
    } else {
      assert(v.is_str());
      rj_tojson<W, std::string_view>::serialize(writer, *v.get_str());
    }
  }
};

template <rjson_writer W, typename T>
struct rj_tojson<W, box<T>> {
  static void serialize(W& writer, const box<T>& v) {
    if (!v) [[unlikely]] {
      (void)writer.Null();
      return;
    }
    rj_tojson<W, T>::serialize(writer, *v);
  }
};

template <rjson_writer W, common_api_type A>
struct rj_tojson<W, A> {
  static void serialize(W& writer, const A& v) {
    writer.StartObject();
    pfr_extension::visit_object(v, [&]<typename Info, typename T>(const T& field) {
      constexpr std::string_view name = Info::name.AsStringView();
      if constexpr (!A::is_mandatory_field(name)) {
        auto* f = noexport::optional_field_get_value(field);
        if (!f)
          return;
        (void)writer.Key(name.data(), name.size());
        rj_tojson<W, std::remove_cvref_t<decltype(*f)>>::serialize(writer, *f);
      } else {
        (void)writer.Key(name.data(), name.size());
        rj_tojson<W, T>::serialize(writer, field);
      }
    });
    writer.EndObject();
  }
};

template <rjson_writer W, oneof_field_api_type A>
struct rj_tojson<W, A> {
  static void serialize(W& writer, const A& v) {
    namespace pfr = boost::pfr;
    static_assert(std::is_same_v<pfr::tuple_element_t<pfr::tuple_size_v<A> - 1, A>, decltype(A::data)>);

    writer.StartObject();
    pfr_extension::visit_object<boost::pfr::tuple_size_v<A> - 1>(
        v, [&]<typename Info, typename T>(const T& field) {
          std::string_view name = Info::name.AsStringView();
          writer.Key(name.data(), name.size());
          rj_tojson<W, T>::serialize(writer, field);
        });
    if (v.data) {
      std::string_view d = v.discriminator_now();
      (void)writer.Key(d.data(), d.size());
      v.data.visit(matcher{
          [](tgbm::nothing_t) { unreachable(); },
          [&]<typename T>(const T& x) { rj_tojson<W, decltype(x.value)>::serialize(writer, x.value); }});
    }
    writer.EndObject();
  }
};

template <rjson_writer W, discriminated_api_type A>
struct rj_tojson<W, A> {
  static void serialize(W& writer, const A& v) {
    static_assert(boost::pfr::tuple_size_v<A> == 1);
    if (!v.data)
      return;
    writer.StartObject();
    std::string_view d = v.discriminator_now();
    (void)writer.Key(d.data(), d.size());
    v.data.visit(matcher{[](tgbm::nothing_t) { unreachable(); },
                         [&]<typename T>(const T& x) { rj_tojson<W, T>::serialize(writer, x); }});
    writer.EndObject();
  }
};

}  // namespace tgbm
