#pragma once

#include <rapidjson/reader.h>
#include <rapidjson/document.h>
#include <tgbm/tools/json_tools/handler_parser/basic_parser.hpp>
#include <tgbm/api/common.hpp>
#include <tgbm/tools/api_utils.hpp>
#include <tgbm/tools/traits.hpp>
#include <tgbm/tools/json_tools/exceptions.hpp>
#include <rapidjson/error/en.h>

namespace tgbm::rapid_dom_parser {

using Doc = rapidjson::GenericValue<rapidjson::UTF8<>>;

template <typename T>
struct rapid_parser {
  // static T parse(Doc& doc);
};

template <std::floating_point T>
struct rapid_parser<T> {
  static void parse(const Doc& doc, T& t) {
    if (doc.IsDouble()) {
      t = doc.GetDouble();
      return;
    }
    TGBM_JSON_PARSE_ERROR;
  }
};

template <std::integral T>
struct rapid_parser<T> {
  static void parse(const Doc& doc, T& t) {
    if (doc.IsInt()) {
      t = doc.GetInt();
      return;
    } else if (doc.IsUint()) {
      t = doc.GetUint();
      return;
    } else if (doc.IsInt64()) {
      t = doc.GetInt64();
      return;
    } else if (doc.IsUint64()) {
      t = doc.GetUint64();
      return;
    }
    TGBM_JSON_PARSE_ERROR;
  }
};

template <>
struct rapid_parser<std::string> {
  static void parse(const Doc& doc, std::string& t) {
    if (doc.IsString()) {
      t = std::string{doc.GetString(), doc.GetStringLength()};
      return;
    }
    TGBM_JSON_PARSE_ERROR;
  }
};

template <>
struct rapid_parser<api::Integer> {
  static void parse(const Doc& doc, api::Integer& t) {
    rapid_parser<std::int64_t>::parse(doc, t.value);
  }
};

template <>
struct rapid_parser<api::Double> {
  static void parse(const Doc& doc, api::Double& t) {
    rapid_parser<double>::parse(doc, t.value);
  }
};

template <>
struct rapid_parser<api::True> {
  static void parse(const Doc& doc, api::True& t) {
    if (!doc.IsBool()) {
      TGBM_JSON_PARSE_ERROR;
    }
    if (!doc.GetBool()) {
      TGBM_JSON_PARSE_ERROR;
    }
  }
};

template <>
struct rapid_parser<bool> {
  static void parse(const Doc& doc, bool& t) {
    if (!doc.IsBool()) {
      TGBM_JSON_PARSE_ERROR;
    }
    t = doc.GetBool();
  }
};

template <typename T>
struct rapid_parser<api::optional<T>> {
  static void parse(const Doc& doc, api::optional<T>& t) {
    if (doc.IsNull()) {
      return;
    }
    rapid_parser<T>::parse(doc, t.emplace());
  }
};

template <typename T>
struct rapid_parser<std::optional<T>> {
  static void parse(const Doc& doc, std::optional<T>& t) {
    if (doc.IsNull()) {
      return;
    }
    rapid_parser<T>::parse(doc, t.emplace());
  }
};

template <typename T>
struct rapid_parser<box<T>> {
  static void parse(const Doc& doc, box<T>& t) {
    if (doc.IsNull()) {
      return;
    }
    rapid_parser<T>::parse(doc, t.emplace());
  }
};

template <typename T>
struct rapid_parser<std::vector<T>> {
  static void parse(const Doc& doc, std::vector<T>& t) {
    if (!doc.IsArray()) {
      TGBM_JSON_PARSE_ERROR;
    }
    t.reserve(doc.Size());
    for (auto it = doc.Begin(); it != doc.End(); it++) {
      rapid_parser<T>::parse(*it, t.emplace_back());
    }
  }
};

template <common_api_type T>
struct rapid_parser<T> {
  static void parse(const Doc& doc, T& t) {
    if (!doc.IsObject()) {
      TGBM_JSON_PARSE_ERROR;
    }
    pfr_extension::visit_object(t, [&]<typename Info, typename Field>(Field& field) {
      constexpr std::string_view name = Info::name;
      rapidjson::GenericStringRef<char> name_ref(name.data(), name.size());
      auto it = doc.FindMember(name_ref);
      if (it == doc.MemberEnd()) {
        if constexpr (T::is_optional_field(name)) {
          return;
        }
        rapid_parser<Field>::parse(Doc{}, field);
      } else {
        rapid_parser<Field>::parse(it->value, field);
      }
    });
  }
};

template <discriminated_api_type T>
struct rapid_parser<T> {
  static void parse(const Doc& doc, T& t) {
    if (doc.IsNull()) {
    }
    if (!doc.IsObject()) {
      TGBM_JSON_PARSE_ERROR;
    }
    constexpr std::string_view discriminator = T::discriminator;
    rapidjson::GenericStringRef<char> discrim_ref(discriminator.data(), discriminator.size());
    auto d_value = doc.FindMember(discrim_ref);
    if (d_value == doc.MemberEnd()) {
      TGBM_JSON_PARSE_ERROR;
    }
    if (!d_value->value.IsString()) {
      TGBM_JSON_PARSE_ERROR;
    }
    std::string_view val{d_value->value.GetString(), d_value->value.GetStringLength()};
    T::discriminate(val, [&]<typename U>() {
      if constexpr (std::same_as<U, void>) {
        TGBM_JSON_PARSE_ERROR;
      } else {
        auto& u = t.data.template emplace<U>();
        rapid_parser<U>::parse(doc, u);
      }
    });
  }
};

template <oneof_field_api_type T>
struct rapid_parser<T> {
  static void parse(const Doc& doc, T& t) {
    constexpr std::size_t N = oneof_field_utils::N<T>;
    if (!doc.IsObject()) {
      TGBM_JSON_PARSE_ERROR;
    }
    pfr_extension::visit_object<N>(t, [&]<typename Info, typename Field>(Field& field) {
      constexpr std::string_view name = Info::name;
      rapidjson::GenericStringRef<char> name_ref(name.data(), name.size());
      auto it = doc.FindMember(name_ref);
      if (it != doc.MemberEnd()) {
        rapid_parser<Field>::parse(it->value, field);
      } else {
        TGBM_JSON_PARSE_ERROR;
      }
    });
    auto& data = t.data;
    for (auto it = doc.MemberBegin(); it != doc.MemberEnd(); ++it) {
      auto key = std::string_view{it->name.GetString(), it->name.GetStringLength()};
      if (!oneof_field_utils::is_required<T>(key)) {
        oneof_field_utils::emplace_field<T, void>(
            t.data, key, [&]<typename Field>(Field& field) { rapid_parser<Field>::parse(it->value, field); },
            [] { TGBM_JSON_PARSE_ERROR; }, [] {});
      }
    }
  }
};

}  // namespace tgbm::rapid_dom_parser

namespace tgbm::json::rapid {

template <typename T>
inline T parse_dom(std::string_view sv) {
  T t;
  rapidjson::Document document;
  rapid_dom_parser::Doc doc;
  document.Parse(sv.data(), sv.size());
  if (document.HasParseError()) {
    TGBM_JSON_PARSE_ERROR;
  }
  rapid_dom_parser::rapid_parser<T>::parse(document, t);
  return t;
}

}  // namespace tgbm::json::rapid
