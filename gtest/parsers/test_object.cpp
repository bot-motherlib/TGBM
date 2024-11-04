#include <parsers/fixtures.hpp>

namespace json_parser::test_cases {

struct TestObject {
  std::strong_ordering operator<=>(const TestObject&) const = default;

  tgbm::api::Integer int_field;
  tgbm::api::Double double_field;
  bool bool_field;
  std::string string_field;
  tgbm::api::optional<std::string> optional_field;
  std::vector<tgbm::api::Integer> array_field;
  std::vector<tgbm::box<TestObject>> nested_objects;

  consteval static bool is_mandatory_field(std::string_view name) {
    return name != "optional_field";
  }
};

JSON_PARSE_TEST(NestedObjectsInArray, {
  // clang-format off
  std::vector<TestObject> expected{
    TestObject{
        .int_field = 1,
        .double_field = 1.1,
        .bool_field = true,
        .string_field = "string1",
        .optional_field = std::string("optional1"),
        .array_field = {1, 2, 3}},
    TestObject{.int_field = 2,
        .double_field = 2.2,
        .bool_field = false,
        .string_field = "string2",
        .optional_field = std::nullopt,
        .array_field = {4, 5, 6}}};
  // clang-format on

  auto json = R"(
   [
      {
        "int_field": 1,
        "double_field": 1.1,
        "bool_field": true,
        "string_field": "string1",
        "optional_field": "optional1",
        "array_field": [1, 2, 3],
        "nested_objects" : []
       },
       {
        "int_field": 2,
        "double_field": 2.2,
        "bool_field": false,
        "string_field": "string2",
        "optional_field": null,
        "array_field": [4, 5, 6],
        "nested_objects": []
        }
    ])";

  auto got = parse_json<std::vector<TestObject>>(json);
  EXPECT_EQ(expected, got);
})

JSON_PARSE_TEST(RequiredFields, {
  TestObject expected{.int_field = 42,
                      .double_field = 3.14,
                      .bool_field = true,
                      .string_field = "test",
                      .optional_field = std::nullopt,
                      .array_field = {}};

  auto json = R"(
            {
                "int_field": 42,
                "double_field": 3.14,
                "bool_field": true,
                "string_field": "test",
                "array_field": [],
                "nested_objects": []
            }
        )";

  auto got = parse_json<TestObject>(json);
  EXPECT_EQ(expected, got);
})

JSON_PARSE_TEST(OptionalFields, {
  // clang-format off
  TestObject expected{.int_field = 100,
    .double_field = 0.01,
    .bool_field = false,
    .string_field = "optional_test",
    .optional_field = std::string("optional_string"),
    .array_field = {}};
  // clang-format on

  auto json = R"(
        {
            "int_field": 100,
            "double_field": 0.01,
            "bool_field": false,
            "string_field": "optional_test",
            "optional_field": "optional_string",
            "array_field": [],
            "nested_objects": []
        }
    )";

  auto got = parse_json<TestObject>(json);
  EXPECT_EQ(expected, got);
})

JSON_PARSE_TEST(NestedObjects, {
  // clang-format off
  TestObject expected{
    .int_field = 1,
    .double_field = 1.1,
    .bool_field = true,
    .string_field = "string1",
    .optional_field = std::nullopt,
    .array_field = {},
    .nested_objects = std::vector<tgbm::box<TestObject>>{tgbm::box<TestObject>{TestObject{
        .int_field = 2,
        .double_field = 2.2,
        .bool_field = false,
        .string_field = "string2",
        .optional_field = std::string("opt"),
        .array_field = {1},
    }}}};
  // clang-format on

  auto json = R"(
            {
                "int_field": 1,
                "double_field": 1.1,
                "bool_field": true,
                "string_field": "string1",
                "array_field" : [],
                "nested_objects": [
                    {
                        "int_field": 2,
                        "double_field": 2.2,
                        "bool_field": false,
                        "string_field": "string2",
                        "optional_field": "opt",
                        "array_field": [1],
                        "nested_objects": []
                    }
                ]
            }
        )";

  auto got = parse_json<TestObject>(json);
  EXPECT_EQ(expected, got);
})

JSON_PARSE_TEST(EmptyStringNumbersArrays, {
  // clang-format off
  TestObject expected{
      .int_field = 0,
      .double_field = 0.0,
      .bool_field = false,
      .string_field = "",
      .optional_field = std::nullopt,
      .array_field = {},
  };

  auto json = R"(
    {
        "int_field": 0,
        "double_field": 0.0,
        "bool_field": false,
        "string_field": "",
        "optional_field": null,
        "array_field": [],
        "nested_objects": []
    }
  )";
  // clang-format on

  auto got = parse_json<TestObject>(json);
  EXPECT_EQ(expected, got);
})

JSON_PARSE_TEST(ExtraFields, {
  auto json = R"(
            {
                "int_field": 1,
                "double_field": 2.2,
                "bool_field": true,
                "string_field": "string",
                "optional_field": "optional",
                "array_field": [1, 2, 3],
                "nested_objects": [
                    {
                        "int_field": 10,
                        "double_field": 20.2,
                        "bool_field": false,
                        "string_field": "nested",
                        "optional_field": null,
                        "array_field": [4, 5, 6],
                        "nested_objects": [],
                        "extra_field1": 100,
                        "extra_field2": 200.2,
                        "extra_field3": "extra",
                        "extra_field4": {
                            "extra_field1": 100,
                            "extra_field2": 200.2,
                            "extra_field3": "extra",
                            "extra_field4": [7, 8, 9]
                        }
                    }
                ],
                "extra_field1": 100,
                "extra_field2": 200.2,
                "extra_field3": "extra",
                "extra_field4": [7, 8, 9]
            }
        )";

  // clang-format off
  TestObject expected{
    .int_field = 1,
    .double_field = 2.2,
    .bool_field = true,
    .string_field = "string",
    .optional_field = std::string("optional"),
    .array_field = {1, 2, 3},
    .nested_objects = {tgbm::box<TestObject>{
        TestObject{
            .int_field = 10,
            .double_field = 20.2,
            .bool_field = false,
            .string_field = "nested",
            .optional_field = std::nullopt,
            .array_field = {4, 5, 6},
            .nested_objects = {}
        }
    }}
    };
  // clang-format on

  auto got = parse_json<TestObject>(json);
  EXPECT_EQ(expected, got);
})

JSON_PARSE_TEST(MissingRequiredFields, {
  auto json = R"(
    {
        "int_field": 42,
        "double_field": 3.14,
        "bool_field": true,
        "array_field": []
    }
  )";
  EXPECT_ANY_THROW(parse_json<TestObject>(json));
})

}  // namespace json_parser::test_cases
