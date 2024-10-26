#include <gtest/gtest.h>

#if 0

  #include "tgbm/tools/json_tools/parser/all.h"
  #include <sstream>
  #include <rapidjson/reader.h>
  #include <rapidjson/document.h>
  #include <rapidjson/istreamwrapper.h>

namespace tests_json_parser {
struct Position {
  int x;
  int y;

  auto operator<=>(const Position&) const = default;
};

struct Geotag {
  bool enabled;
  std::string name;
  Position position;

  auto operator<=>(const Geotag&) const = default;
};

struct Image {
  tgbm::box<Geotag> geotag;
  std::string name;

  auto operator<=>(const Image&) const = default;
};

}  // namespace tests_json_parser

using namespace tests_json_parser;

TEST(ParserTests, PlainStruct) {
  std::string json_str = R"(
    {
      "x" : 1,
      "y" : 2
    }
  )";
  auto pos = tgbm::json::handler_parser::from_json<Position>(json_str);
  EXPECT_EQ(pos.x, 1);
  EXPECT_EQ(pos.y, 2);
}

TEST(ParserTests, Substructure) {
  std::string json_str = R"(
    {
        "position" : { "x": 10, "y" : 20 },
        "enabled": false,
        "name": "some_pos"
    }
  )";
  std::stringstream ss(json_str);
  rapidjson::IStreamWrapper isw(ss);
  Geotag x;
  tgbm::json::ParserHandler<Geotag> handler(x);
  rapidjson::Reader reader;
  auto result = reader.Parse(isw, handler);
  EXPECT_EQ(x.enabled, false);
  EXPECT_EQ(x.name, "some_pos");
  EXPECT_EQ(x.position.x, 10);
  EXPECT_EQ(x.position.y, 20);
}

TEST(ParserTests, ExtraFields) {
  std::string json_str = R"(
    {
        "position" : { "x": 10, "y" : 20 },
        "enabled": false,
        "name": "some_pos",
        "some_extra": {
            "id": 132
        }
    }
  )";
  auto x = tgbm::json::handler_parser::from_json<Geotag>(json_str);
  EXPECT_EQ(x.enabled, false);
  EXPECT_EQ(x.name, "some_pos");
  EXPECT_EQ(x.position.x, 10);
  EXPECT_EQ(x.position.y, 20);
}

TEST(ParserTests, BoxStructure) {
  std::string json_str = R"(
    {
      "name": "test",
      "geotag": {
          "position" : { "x": 10, "y" : 20 },
          "enabled": false,
          "name": "some_pos"
      }
    }
      
  )";
  auto got = tgbm::json::handler_parser::from_json<Image>(json_str);
  auto expected = Image{
      .geotag = Geotag{.enabled = false, .name = "some_pos", .position = {.x = 10, .y = 20}}, .name = "test"};
  EXPECT_EQ(got, expected);
}

#endif
