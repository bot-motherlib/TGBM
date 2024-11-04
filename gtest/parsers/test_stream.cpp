#include <parsers/fixtures.hpp>

namespace json_parser::test_stream {

static std::string_view STR(std::string_view text) {
  text.remove_suffix(1);
  text.remove_prefix(1);
  return text;
}

struct TestObject {
  std::strong_ordering operator<=>(const TestObject&) const = default;

  tgbm::api::optional<tgbm::api::Integer> int_field;
  tgbm::api::optional<tgbm::api::Double> double_field;
  tgbm::api::optional<bool> bool_field;
  tgbm::api::optional<std::string> string_field;
  tgbm::api::optional<std::vector<tgbm::api::Integer>> array_field;
  tgbm::api::optional<std::vector<tgbm::box<TestObject>>> nested_objects;

  consteval static bool is_mandatory_field(std::string_view name) {
    return false;
  }
};

using parser = tgbm::json::stream_parser;

TEST(StreamParsing, Int) {
  TestObject expected{.int_field = {1}};
  TestObject got;
  parser parser(got);
  parser.feed(STR(R"(
{"int_field": 1
)"),
              false);
  parser.feed(STR(R"(
23}
)"),
              true);
}

TEST(StreamParsing, DoublePointLeft) {
  TestObject expected{.double_field = {21.12}};
  TestObject got;
  parser parser(got);
  parser.feed(STR(R"(
{"double_field": 21.
)"),
              false);
  parser.feed(STR(R"(
12}
)"),
              true);

  EXPECT_EQ(expected, got);
}

TEST(StreamParsing, DoublePointRight) {
  TestObject expected{.double_field = {21.12}};
  TestObject got;
  parser parser(got);
  parser.feed(STR(R"(
{"double_field": 21
)"),
              false);
  parser.feed(STR(R"(
.12}
)"),
              true);

  EXPECT_EQ(expected, got);
}

TEST(StreamParsing, Bool) {
  TestObject expected{.bool_field = false};
  std::string_view full_json = STR(R"(
{"bool_field": false}
)");
  for (auto i = 1; i < 5; i++) {
    std::string_view first = full_json.substr(0, full_json.size() - i);
    std::string_view second = full_json.substr(full_json.size() - i, i);
    TestObject got;
    parser parser(got);
    parser.feed(first, false);
    parser.feed(second, true);

    EXPECT_EQ(expected, got);
  }
}

TEST(StreamParsing, Null) {
  TestObject expected{};
  std::string_view full_json = STR(R"(
{"string_field": null}
)");
  for (auto i = 1; i < 4; i++) {
    std::string_view first = full_json.substr(0, full_json.size() - i);
    std::string_view second = full_json.substr(full_json.size() - i, i);
    TestObject got;
    parser parser(got);
    parser.feed(first, false);
    parser.feed(second, true);

    EXPECT_EQ(expected, got);
  }
}

constexpr std::string_view kStringField = "string_field";

TEST(StreamParsing, Key2Part) {
  TestObject expected{.string_field = {"test"}};
  std::string_view first = STR(R"(
{"string_
)");
  std::string_view second = STR(R"(
field": "test"}    
  )");

  TestObject got;
  parser parser(got);
  parser.feed(first, false);
  parser.feed(second, true);
  EXPECT_EQ(expected, got);
}

TEST(StreamParsing, Key3Part) {
  TestObject expected{.string_field = {"test"}};
  std::string_view first = STR(R"(
{"str
)");
  std::string_view second = STR(R"(
ing_
)");
  std::string_view third = STR(R"(
field": "test"}    
  )");

  TestObject got;
  parser parser(got);
  parser.feed(first, false);
  parser.feed(second, false);
  parser.feed(third, true);
  EXPECT_EQ(expected, got);
}

TEST(StreamParsing, String3Part) {
  TestObject expected{.string_field = {"test"}};
  std::string_view first = STR(R"(
{"string_field":"t
)");
  std::string_view second = STR(R"(
es
)");
  std::string_view third = STR(R"(
t"}    
)");

  TestObject got;
  parser parser(got);
  parser.feed(first, false);
  parser.feed(second, false);
  parser.feed(third, true);
  EXPECT_EQ(expected, got);
}

TEST(StreamParser, NestedObjectsInArray) {
  // clang-format off
  std::vector<TestObject> expected{
    TestObject{
        .int_field = {1},
        .double_field = {1.1},
        .bool_field = {true},
        .string_field = {"string1"},
        .array_field = {{1, 2, 3}},
        .nested_objects = {{}}},
    TestObject{.int_field = {2},
        .double_field = {2.2},
        .bool_field = {false},
        .string_field = {"string2"},
        .array_field = {{4, 5, 6}},
        .nested_objects = {{}}}};
  // clang-format on

  auto json = std::string_view(R"(
   [
      {
        "int_field": 1,
        "double_field": 1.1,
        "bool_field": true,
        "string_field": "string1",
        "array_field": [1, 2, 3],
        "nested_objects" : []
       },
       {
        "int_field": 2,
        "double_field": 2.2,
        "bool_field": false,
        "string_field": "string2",
        "array_field": [4, 5, 6],
        "nested_objects": []
        }
    ])");

  for (std::size_t i = 24; i < 25; i++) {
    std::vector<TestObject> got;

    parser parser(got);
    std::string_view first = json.substr(0, i);
    std::string_view second = json.substr(i);

    parser.feed(first, false);
    parser.feed(second, true);
    EXPECT_EQ(expected, got) << "failed on : " << i;
  }
}

}  // namespace json_parser::test_stream
