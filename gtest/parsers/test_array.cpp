#include <numeric>
#include <parsers/fixtures.hpp>
#include "tgbm/api/Integer.hpp"

namespace json_parser::test_array {

struct SimpleObject {
  tgbm::api::Integer int_field;
  tgbm::api::Double double_field;
  bool bool_field;
  std::string string_field;
  tgbm::api::optional<std::string> optional_field;
  tgbm::api::optional<tgbm::api::arrayof<int64_t>> array_field;
  tgbm::api::optional<tgbm::api::arrayof<tgbm::box<SimpleObject>>> nested_objects;

  std::strong_ordering operator<=>(const SimpleObject&) const = default;

  consteval static bool is_mandatory_field(std::string_view name) {
    return tgbm::string_switch<bool>(name)
        .case_("int_field", true)
        .case_("double_field", true)
        .case_("bool_field", true)
        .case_("string_field", true)
        .or_default(false);
  }
};

JSON_PARSE_TEST(IntegerArray, std::vector<tgbm::api::Integer>) {
  std::vector<tgbm::api::Integer> expected = {1, 2, 3, 4, 5};

  auto json = R"(
            [1, 2, 3, 4, 5]
        )";

  auto got = parse_json(json);
  EXPECT_EQ(expected, got);
}

JSON_PARSE_TEST(ObjectArray, std::vector<SimpleObject>) {
  // clang-format off
    std::vector<SimpleObject> expected{
        SimpleObject{
            .int_field = 1,
            .double_field = 1.1,
            .bool_field = true,
            .string_field = "object1",
            .optional_field = std::nullopt,
            .array_field = {},
            .nested_objects = {}
        },
        SimpleObject{
            .int_field = 2,
            .double_field = 2.2,
            .bool_field = false,
            .string_field = "object2",
            .optional_field = std::string("optional"),
            .array_field = tgbm::api::arrayof<int64_t>{1, 2},
            .nested_objects = {}
        }
    };
  // clang-format on

  auto json = R"(
    [
        {
            "int_field": 1,
            "double_field": 1.1,
            "bool_field": true,
            "string_field": "object1"
        },
        {
            "int_field": 2,
            "double_field": 2.2,
            "bool_field": false,
            "string_field": "object2",
            "optional_field": "optional",
            "array_field": [1, 2]
        }
    ]
  )";

  auto got_fast_parse = parse_json(json);

  EXPECT_EQ(expected, got_fast_parse);
}

JSON_PARSE_TEST(EmptyArray, std::vector<int>) {
  std::vector<int> expected = {};

  auto json = R"(
            []
        )";

  auto got = parse_json(json);
  EXPECT_EQ(expected, got);
}

JSON_PARSE_TEST(MultidimensionalArray, std::vector<std::vector<int>>) {
  std::vector<std::vector<int>> expected{
      {1, 2, 3},
      {4, 5, 6},
      {7, 8, 9},
  };

  auto json = R"(
    [
        [1, 2, 3],
        [4, 5, 6],
        [7, 8, 9]
    ]
  )";

  auto got = parse_json(json);
  EXPECT_EQ(expected, got);
}

JSON_PARSE_TEST(ObjectArrayWithOptionalFields, std::vector<SimpleObject>) {
  // clang-format off
  std::vector<SimpleObject> expected{
        SimpleObject{
            .int_field = 1,
            .double_field = 1.1,
            .bool_field = true,
            .string_field = "object1",
            .optional_field = std::string("opt1"),
            .array_field = {},
            .nested_objects = {}
        },
        SimpleObject{
            .int_field = 2,
            .double_field = 2.2,
            .bool_field = false,
            .string_field = "object2",
            .optional_field = std::nullopt,
            .array_field = tgbm::api::arrayof<int64_t>{1},
            .nested_objects = {}
        }
  };
  // clang-format on

  auto json = R"(
            [
                {
                    "int_field": 1,
                    "double_field": 1.1,
                    "bool_field": true,
                    "string_field": "object1",
                    "optional_field": "opt1"
                },
                {
                    "int_field": 2,
                    "double_field": 2.2,
                    "bool_field": false,
                    "string_field": "object2",
                    "array_field": [1]
                }
            ]
        )";

  auto got = parse_json(json);
  EXPECT_EQ(expected, got);
}

JSON_PARSE_TEST(ArrayWithNull, std::vector<tgbm::api::optional<int>>) {
  std::vector<tgbm::api::optional<int>> expected{1, 2, std::nullopt, 4};

  auto json = R"(
            [1, 2, null, 4]
        )";

  auto got = parse_json(json);
  EXPECT_EQ(expected, got);
}

JSON_PARSE_TEST(LargeArray, std::vector<int>) {
  std::vector<int> expected(10);
  std::iota(expected.begin(), expected.end(), 0);
  auto json = fmt::format("[{}]", fmt::join(expected, ","));
  auto got = parse_json(json);
  EXPECT_EQ(expected, got);
}

JSON_PARSE_TEST(MissingArrayElement, std::vector<int>) {
  auto json = R"(
            [
                1,
                2,
                3,
                "string"
            ]
        )";

  EXPECT_ANY_THROW(parse_json(json));
}

}  // namespace json_parser::test_array
