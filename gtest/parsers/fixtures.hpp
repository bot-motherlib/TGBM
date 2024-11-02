#pragma once
#include <gtest/gtest.h>
#include <gtest_formater.hpp>

#include "boost/json/value.hpp"
#include "tgbm/api/common.hpp"
#include "tgbm/tools/json_tools/all.hpp"

struct DomRapid : testing::Test {
  template <typename T>
  static T parse_json(std::string_view json) {
    rapidjson::Document document;
    document.Parse(json.data(), json.size());
    if (document.HasParseError()) {
      TGBM_JSON_PARSE_ERROR;
    }
    return tgbm::json::from_json<T, rapidjson::GenericValue<rapidjson::UTF8<>>>(document);
  }
};

struct DomBoost : testing::Test {
  template <typename T>
  static T parse_json(std::string_view json) {
    ::boost::json::value j = boost::json::parse(json);
    return tgbm::json::from_json<T>(j);
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
    return tgbm::json::boost::parse_generator<T>(json);
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
