#pragma once
#include <gtest/gtest.h>
#include <gtest_formater.hpp>

#include <boost/json/value.hpp>
#include <tgbm/api/common.hpp>

#include <tgbm/jsons/boostjson_dom_traits.hpp>
#include <tgbm/jsons/boostjson_sax_producer.hpp>
#include "../../benchmarks/boost_parse_handler.hpp"
#include "../../benchmarks/handler_parser/all.hpp"
#include "../../benchmarks/rapid_parse_handler.hpp"
#include <tgbm/json.hpp>
#include <tgbm/jsons/sax_parser.hpp>
#include <tgbm/jsons/dom_parser.hpp>

#include <tgbm/jsons/rapidjson_dom_traits.hpp>
#include <tgbm/jsons/errors.hpp>
#include <tgbm/jsons/stream_parser.hpp>

struct DomRapid {
  template <typename T>
  static T parse_json(std::string_view json) {
    rapidjson::Document document;
    document.Parse(json.data(), json.size());
    if (document.HasParseError()) {
      TGBM_JSON_PARSE_ERROR;
    }
    return tgbm::from_json<T, rapidjson::GenericValue<rapidjson::UTF8<>>>(document);
  }
};

struct DomBoost {
  template <typename T>
  static T parse_json(std::string_view json) {
    ::boost::json::value j = boost::json::parse(json);
    return tgbm::from_json<T>(j);
  }
};

struct HandlerRapid {
  template <typename T>
  static T parse_json(std::string_view json) {
    return tgbm::json::rapid::parse_handler<T>(json);
  }
};

struct HandlerBoost {
  template <typename T>
  static T parse_json(std::string_view json) {
    return tgbm::json::boost::parse_handler<T>(json);
  }
};

struct GeneratorBoost {
  template <typename T>
  static T parse_json(std::string_view json) {
    T v;
    tgbm::json::stream_parser s(v);
    s.feed(json, true);
    return v;
  }
};

// clang-format off

#define JSON_PARSE_TEST_NAME(Name) JsonParseTest##Name

#define JSON_INFO_NAME(Method) test_info##Method

#define JSON_REG_DECL(Method) static ::testing::TestInfo* JSON_INFO_NAME(Method)

#define JSON_REG_DECL_ALL()    \
JSON_REG_DECL(DomRapid);       \
JSON_REG_DECL(DomBoost);       \
JSON_REG_DECL(HandlerRapid);   \
JSON_REG_DECL(HandlerBoost);   \
JSON_REG_DECL(GeneratorBoost)

#define JSON_PARSE_REG(Name, Type, Method)                                                    \
::testing::TestInfo* JSON_PARSE_TEST_NAME(Name)::JSON_INFO_NAME(Method)                       \
    = ::testing::RegisterTest<>                                                               \
    (#Method, #Name, nullptr, nullptr, __FILE__, __LINE__,                                    \
    []() -> testing::Test* {                                                                  \
      return new JSON_PARSE_TEST_NAME(Name){&Method::parse_json<Type>};                       \
    })

#define JSON_PARSE_REG_ALL(Name, Type)     \
JSON_PARSE_REG(Name, Type, DomRapid);      \
JSON_PARSE_REG(Name, Type, DomBoost);      \
JSON_PARSE_REG(Name, Type, HandlerRapid);  \
JSON_PARSE_REG(Name, Type, HandlerBoost);  \
JSON_PARSE_REG(Name, Type, GeneratorBoost)

#define JSON_PARSE_TEST(Name, Type) struct JSON_PARSE_TEST_NAME(Name) : testing::Test {       \
  using parse_json_t = Type(*)(std::string_view);                                             \
  void TestBody() override;                                                                   \
  JSON_PARSE_TEST_NAME(Name)(parse_json_t parse_json) : parse_json(parse_json) {}             \
private:                                                                                      \
  parse_json_t parse_json;                                                                    \
  JSON_REG_DECL_ALL();                                                                        \
};                                                                                            \
JSON_PARSE_REG_ALL(Name, Type);                                                               \
void JSON_PARSE_TEST_NAME(Name)::TestBody()

// clang-format on
