#pragma once
#include <gtest/gtest.h>
#include <gtest_formater.hpp>

#include "boost/json/value.hpp"
#include "tgbm/api/common.hpp"

#include "tgbm/jsons/boost_parse_dom.hpp"
#include "tgbm/jsons/sax.hpp"
#include "tgbm/jsons/boost_parse_handler.hpp"
#include "tgbm/json.hpp"
#include "tgbm/jsons/generator_parser/all.hpp"
#include "tgbm/jsons/handler_parser/all.hpp"
#include "tgbm/jsons/parse_dom/all.hpp"
#include "tgbm/jsons/rapid_parse_handler.hpp"
#include "tgbm/jsons/rapid_parse_dom.hpp"
#include "tgbm/jsons/exceptions.hpp"
#include "tgbm/jsons/stream_parser.hpp"

struct DomRapid : testing::Test {
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

struct DomBoost : testing::Test {
  template <typename T>
  static T parse_json(std::string_view json) {
    ::boost::json::value j = boost::json::parse(json);
    return tgbm::from_json<T>(j);
  }
};

struct HandlerRapid : testing::Test {
  template <typename T>
  static T parse_json(std::string_view json) {
    return tgbm::json::rapid::parse_handler<T>(json);
  }
};

struct HandlerBoost : testing::Test {
  template <typename T>
  static T parse_json(std::string_view json) {
    return tgbm::json::boost::parse_handler<T>(json);
  }
};

struct GeneratorBoost : testing::Test {
  template <typename T>
  static T parse_json(std::string_view json) {
    T v;
    tgbm::json::stream_parser s(v);
    s.feed(json, true);
    return v;
  }
};

// clang-format off

#define JSON_PARSE_TEST(Name, ...)             \
  TEST_F(DomRapid, Name) __VA_ARGS__           \
  TEST_F(DomBoost, Name) __VA_ARGS__           \
  TEST_F(HandlerRapid, Name) __VA_ARGS__       \
  TEST_F(HandlerBoost, Name)  __VA_ARGS__      \
  TEST_F(GeneratorBoost, Name) __VA_ARGS__

// clang-format on
