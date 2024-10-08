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

  consteval static bool is_optional_field(std::string_view name) {
    return tgbm::utils::string_switch<std::optional<bool>>(name)
        .case_("int_field", true)
        .case_("double_field", true)
        .case_("bool_field", true)
        .case_("string_field", true)
        .case_("optional_field", true)
        .case_("array_field", true)
        .case_("nested_objects", true)
        .or_default(std::nullopt)
        .value();
  }
};

using parser = tgbm::json::boost::stream_parser<TestObject>;
using array_parser = tgbm::json::boost::stream_parser<std::vector<TestObject>>;

TEST(StreamParsing, Int) {
  TestObject expected{.int_field = {1}};
  TestObject got;
  parser parser(got);
  parser.parse(STR(R"(
{"int_field": 1
)"), false);
  parser.parse(STR(R"(
23}
)"),
               true);
}

TEST(StreamParsing, DoublePointLeft) {
  TestObject expected{.double_field = {21.12}};
  TestObject got;
  parser parser(got);
  parser.parse(STR(R"(
{"double_field": 21.
)"), false);
  parser.parse(STR(R"(
12}
)"),
               true);

  EXPECT_EQ(expected, got);
}

TEST(StreamParsing, DoublePointRight) {
  TestObject expected{.double_field = {21.12}};
  TestObject got;
  parser parser(got);
  parser.parse(STR(R"(
{"double_field": 21
)"), false);
  parser.parse(STR(R"(
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
    parser.parse(first, false);
    parser.parse(second, true);

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
    parser.parse(first, false);
    parser.parse(second, true);

    EXPECT_EQ(expected, got);
  }
}

constexpr std::string_view kStringField = "string_field";

TEST(StreamParsing, Key) {
  TestObject expected{.string_field = {"test"}};
  std::string_view first = STR(R"(
{"string_
)");
  std::string_view second = STR(R"(
field": "test"}    
  )");

  TestObject got;
  parser parser(got);
  parser.parse(first, false);
  parser.parse(second, true);
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

    array_parser parser(got);
    std::string_view first = json.substr(0, i);
    std::string_view second = json.substr(i);

    parser.parse(first, false);
    parser.parse(second, true);
    EXPECT_EQ(expected, got) << "failed on : " << i;
  }
}

}  // namespace json_parser::test_stream
