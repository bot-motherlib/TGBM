#include <parsers/fixtures.hpp>

namespace json_parser::test_optional {

// Пример простого объекта для тестирования
struct SimpleType {
  std::strong_ordering operator<=>(const SimpleType&) const = default;
  bool operator==(const SimpleType&) const = default;

  std::string name;
  int value;

  // Здесь ни одно поле не является optional или tgbm::box
  consteval static bool is_optional_field(std::string_view name) {
    return tgbm::utils::string_switch<std::optional<bool>>(name)
        .case_("name", false)
        .case_("value", false)
        .or_default(std::nullopt)
        .value();
  }
};

// Пример объекта с optional
struct ComplexOptionalType {
  std::strong_ordering operator<=>(const ComplexOptionalType&) const = default;
  bool operator==(const ComplexOptionalType&) const = default;

  std::string name;
  tgbm::api::optional<int> optional_number;
  tgbm::api::optional<SimpleType> optional_object;
  tgbm::api::optional<std::vector<int>> optional_array;

  // Указываем, что только optional-поля отмечаются как optional
  consteval static bool is_optional_field(std::string_view name) {
    return tgbm::utils::string_switch<std::optional<bool>>(name)
        .case_("name", false)
        .case_("optional_number", true)
        .case_("optional_object", true)
        .case_("optional_array", true)
        .or_default(std::nullopt)
        .value();
  }
};

// Проверка парсинга optional-поля с присутствующим значением
JSON_PARSE_TEST(OptionalPresentValue, {
  ComplexOptionalType expected;
  expected.name = "TestObject";
  expected.optional_number = 42;

  auto json = R"(
            {
                "name": "TestObject",
                "optional_number": 42
            }
        )";

  auto got = parse_json<ComplexOptionalType>(json);
  EXPECT_EQ(expected, got);
})

// Проверка парсинга optional-поля при отсутствии значения
JSON_PARSE_TEST(OptionalAbsentValue, {
  ComplexOptionalType expected;
  expected.name = "TestObject";
  expected.optional_number = std::nullopt;

  auto json = R"(
            {
                "name": "TestObject"
            }
        )";

  auto got = parse_json<ComplexOptionalType>(json);
  EXPECT_EQ(expected, got);
})

// Проверка обработки невалидного значения в optional (например, строка вместо числа)
JSON_PARSE_TEST(InvalidOptionalValue, {
  auto json = R"(
            {
                "name": "TestObject",
                "optional_number": "invalid_number"
            }
        )";

  EXPECT_ANY_THROW(parse_json<ComplexOptionalType>(json));
})

// Проверка парсинга optional, содержащего сложный объект
JSON_PARSE_TEST(OptionalComplexObject, {
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

  auto got = parse_json<ComplexOptionalType>(json);
  EXPECT_EQ(expected, got);
})

// Проверка обработки optional, содержащего массив
JSON_PARSE_TEST(OptionalArray, {
  ComplexOptionalType expected;
  expected.name = "TestObject";
  expected.optional_array = std::vector<int>{1, 2, 3};

  auto json = R"(
            {
                "name": "TestObject",
                "optional_array": [1, 2, 3]
            }
        )";

  auto got = parse_json<ComplexOptionalType>(json);
  EXPECT_EQ(expected, got);
})

}  // namespace json_parser::test_optional
