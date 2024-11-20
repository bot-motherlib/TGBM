#pragma once

#include <fmt/base.h>

#include <rapidjson/prettywriter.h>

#include <tgbm/jsons/rapidjson_serialize_sax.hpp>
#include <tgbm/utils/traits.hpp>

namespace fmt {

template <tgbm::api_type T>
struct formatter<T> {
  bool pretty = false;

  constexpr const char* parse(auto& ctx) {
    const char* it = ctx.begin();
    if (it == ctx.end() || *it == '}')
      return it;
    if (*it == 'p') {
      pretty = true;
      ++it;
    } else if (*it == '!' && (it + 1 != ctx.end()) && *(it + 1) == 'p') {
      pretty = false;
      it += 2;
    } else {
      throw format_error("Invalid format, valid is {}, {:p} or {:!p}");
    }
    if (it == ctx.end() || *it != '}')
      throw format_error("Invalid format, valid is {}, {:p} or {:!p}");
    return it;
  }

  template <typename Ctx>
  auto format(const T& value, Ctx& ctx) const -> decltype(ctx.out()) {
    struct rj_printbuf {
      std::remove_cvref_t<decltype(ctx.out())> out;

      using Ch = char;

      void Put(char c) {
        *out = c;
        ++out;
      }
      void Flush() {
      }
    };

    rj_printbuf buf(ctx.out());
    if (pretty) {
      rapidjson::PrettyWriter w(buf);
      tgbm::rj_tojson<decltype(w), T>::serialize(w, value);
    } else {
      rapidjson::Writer w(buf);
      tgbm::rj_tojson<decltype(w), T>::serialize(w, value);
    }
    return buf.out;
  }
};

}  // namespace fmt
