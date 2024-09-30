#pragma once
#include <gtest/gtest.h>
#include <gtest_formater.hpp>
#include <tgbm/api/common.hpp>
#include <tgbm/tools/json_tools/all.hpp>

struct DomRapid : testing::Test{
  template <typename T>
  static T parse_json(std::string_view json){
    return tgbm::json::rapid::parse_dom<T>(json);
  }
};

struct DomBoost : testing::Test{
  template <typename T>
  static T parse_json(std::string_view json){
    return tgbm::json::boost::parse_dom<T>(json);
  }
};

struct HandlerRapid : testing::Test{
  template <typename T>
  static T parse_json(std::string_view json){
    return tgbm::json::rapid::parse_handler<T>(json);
  }
};

struct HandlerBoost : testing::Test{
  template <typename T>
  static T parse_json(std::string_view json){
     return tgbm::json::boost::parse_handler<T>(json);
  }
};

struct GeneratorBoost : testing::Test{
  template <typename T>
  static T parse_json(std::string_view json){
     return tgbm::json::boost::parse_generator<T>(json);
  }
};

#define JSON_PARSE_TEST(Name, ...) \
TEST_F(DomRapid, Name) __VA_ARGS__ \
TEST_F(DomBoost, Name) __VA_ARGS__ \
TEST_F(HandlerRapid, Name) __VA_ARGS__ \
TEST_F(HandlerBoost, Name) __VA_ARGS__ \
TEST_F(GeneratorBoost, Name) __VA_ARGS__ \

