#include <parsers/fixtures.hpp>

namespace json_parser::test_optional {

struct SimpleType {
  std::strong_ordering operator<=>(const SimpleType&) const = default;
  bool operator==(const SimpleType&) const = default;

  std::string name;
  int value;

  consteval static bool is_mandatory_field(std::string_view name) {
    return true;
  }
};

struct ComplexOptionalType {
  std::strong_ordering operator<=>(const ComplexOptionalType&) const = default;
  bool operator==(const ComplexOptionalType&) const = default;

  std::string name;
  tgbm::api::optional<int> optional_number;
  tgbm::api::optional<SimpleType> optional_object;
  tgbm::api::optional<std::vector<int>> optional_array;

  consteval static bool is_mandatory_field(std::string_view name) {
    return name == "name";
  }
};

JSON_PARSE_TEST(OptionalPresentValue, ComplexOptionalType) {
  ComplexOptionalType expected;
  expected.name = "TestObject";
  expected.optional_number = 42;

  auto json = R"(
            {
                "name": "TestObject",
                "optional_number": 42
            }
        )";

  auto got = parse_json(json);
  EXPECT_EQ(expected, got);
}

JSON_PARSE_TEST(OptionalAbsentValue, ComplexOptionalType) {
  ComplexOptionalType expected;
  expected.name = "TestObject";
  expected.optional_number = std::nullopt;

  auto json = R"(
            {
                "name": "TestObject"
            }
        )";

  auto got = parse_json(json);
  EXPECT_EQ(expected, got);
}

JSON_PARSE_TEST(InvalidOptionalValue, ComplexOptionalType) {
  auto json = R"(
            {
                "name": "TestObject",
                "optional_number": "invalid_number"
            }
        )";

  EXPECT_ANY_THROW(parse_json(json));
}

JSON_PARSE_TEST(OptionalComplexObject, ComplexOptionalType) {
  ComplexOptionalType expected;
  expected.name = "TestObject";
  expected.optional_object = SimpleType{"NestedObject", 100};

  auto json = R"(
            {
                "name": "TestObject",
                "optional_object": {
                    "name": "NestedObject",
                    "value": 100
                }
            }
        )";

  auto got = parse_json(json);
  EXPECT_EQ(expected, got);
}

JSON_PARSE_TEST(OptionalArray, ComplexOptionalType) {
  ComplexOptionalType expected;
  expected.name = "TestObject";
  expected.optional_array = std::vector<int>{1, 2, 3};

  auto json = R"(
            {
                "name": "TestObject",
                "optional_array": [1, 2, 3]
            }
        )";

  auto got = parse_json(json);
  EXPECT_EQ(expected, got);
}

}  // namespace json_parser::test_optional
