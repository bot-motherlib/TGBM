#include <parsers/fixtures.hpp>

using namespace std::literals;

struct object1 {
  std::string name;
  int age;
  std::strong_ordering operator<=>(const object1&) const = default;

  static consteval bool is_mandatory_field(std::string_view key) {
    return true;
  }
};

struct object2 {
  std::string description;
  std::vector<std::string> tags;
  std::strong_ordering operator<=>(const object2&) const = default;

  static consteval bool is_mandatory_field(std::string_view key) {
    return true;
  }
};

struct custom {
  std::string info;
  tgbm::api::optional<std::string> details;

  static consteval bool is_mandatory_field(std::string_view key) {
    return key == "info";
  }

  std::strong_ordering operator<=>(const custom&) const = default;
};

struct CustomStruct {
  struct optional_bool {
    bool value;
    std::strong_ordering operator<=>(const optional_bool&) const = default;
  };
  struct optional_double {
    double value;
    std::strong_ordering operator<=>(const optional_double&) const = default;
  };
  struct optional_array {
    std::vector<int> value;
    std::strong_ordering operator<=>(const optional_array&) const = default;
  };

  struct optional_object1 {
    object1 value;
    std::strong_ordering operator<=>(const optional_object1&) const = default;
  };
  struct optional_object2 {
    object2 value;
    std::strong_ordering operator<=>(const optional_object2&) const = default;
  };
  struct optional_custom {
    custom value;
    std::strong_ordering operator<=>(const optional_custom&) const = default;
  };

  std::string mandatory_string;
  int mandatory_int;
  tgbm::box_union<optional_bool, optional_double, optional_array, optional_object1, optional_object2,
                  optional_custom>
      data;

  std::strong_ordering operator<=>(const CustomStruct&) const = default;

  enum struct type_e {
    k_optional_bool,
    k_optional_double,
    k_optional_array,
    k_optional_object1,
    k_optional_object2,
    k_optional_custom,
    nothing
  };

  static constexpr size_t variant_size = size_t(type_e::nothing);

  static constexpr decltype(auto) discriminate_field(std::string_view val, auto&& visiter) {
    if (val == "optional_bool")
      return visiter.template operator()<optional_bool>();
    if (val == "optional_double")
      return visiter.template operator()<optional_double>();
    if (val == "optional_array")
      return visiter.template operator()<optional_array>();
    if (val == "optional_object1")
      return visiter.template operator()<optional_object1>();
    if (val == "optional_object2")
      return visiter.template operator()<optional_object2>();
    if (val == "optional_custom")
      return visiter.template operator()<optional_custom>();
    return visiter.template operator()<void>();
  }
  type_e type() const {
    return static_cast<type_e>(data.index());
  }
  std::string_view discriminator_now() const noexcept {
    using From = CustomStruct::type_e;
    switch (type()) {
      case From::k_optional_bool:
        return "optional_bool";
      case From::k_optional_double:
        return "optional_double";
      case From::k_optional_array:
        return "optional_array";
      case From::k_optional_object1:
        return "optional_object1";
      case From::k_optional_object2:
        return "optional_object2";
      case From::k_optional_custom:
        return "optional_custom";
      case From::nothing:
        return "nothing";
    }
  }
};

JSON_PARSE_TEST(TestOneOfFieldOptionalBool, {
  CustomStruct expected;
  expected.mandatory_string = "TestWithBool";
  expected.mandatory_int = 100;
  expected.data = CustomStruct::optional_bool{true};

  auto json = R"(
    {
      "mandatory_string": "TestWithBool",
      "mandatory_int": 100,
      "optional_bool": true
    }
  )";

  auto got = parse_json<CustomStruct>(json);
  EXPECT_EQ(expected, got);
})

JSON_PARSE_TEST(TestOneOfFieldOptionalDouble, {
  CustomStruct expected;
  expected.mandatory_string = "TestWithDouble";
  expected.mandatory_int = 100;
  expected.data = CustomStruct::optional_double{3.14};

  auto json = R"(
    {
      "mandatory_string": "TestWithDouble",
      "mandatory_int": 100,
      "optional_double": 3.14
    }
  )";

  auto got = parse_json<CustomStruct>(json);
  EXPECT_EQ(expected, got);
})

JSON_PARSE_TEST(TestOneOfFieldOptionalArray, {
  CustomStruct expected;
  expected.mandatory_string = "TestWithArray";
  expected.mandatory_int = 300;
  expected.data = CustomStruct::optional_array{std::vector<int>{1, 2, 3, 4, 5}};

  auto json = R"(
    {
      "mandatory_string": "TestWithArray",
      "mandatory_int": 300,
      "optional_array": [1, 2, 3, 4, 5]
    }
  )";

  auto got = parse_json<CustomStruct>(json);
  EXPECT_EQ(expected, got);
})

JSON_PARSE_TEST(TestOneOfFieldObject1, {
  CustomStruct expected;
  expected.mandatory_string = "TestString";
  expected.mandatory_int = 123;

  // Заполняем опциональное поле optional_object1
  expected.data = CustomStruct::optional_object1{object1{"John Doe", 30}};

  // JSON строка
  auto json = R"(
    {
      "mandatory_string": "TestString",
      "mandatory_int": 123,
      "optional_object1": {
        "name": "John Doe",
        "age": 30
      }
    }
  )";

  // Парсинг JSON
  auto got = parse_json<CustomStruct>(json);

  // Проверка на равенство
  EXPECT_EQ(expected, got);
})

JSON_PARSE_TEST(TestOneOfFieldObject2, {
  CustomStruct expected;
  expected.mandatory_string = "TestWithObject2";
  expected.mandatory_int = 500;
  expected.data = CustomStruct::optional_object2{"Test description", {"tag1", "tag2", "tag3"}};

  auto json = R"(
    {
      "mandatory_string": "TestWithObject2",
      "mandatory_int": 500,
      "optional_object2": {
        "description": "Test description",
        "tags": ["tag1", "tag2", "tag3"]
      }
    }
  )";

  auto got = parse_json<CustomStruct>(json);
  EXPECT_EQ(expected, got);
})

JSON_PARSE_TEST(TestOneOfFieldCustom, {
  CustomStruct expected;
  expected.mandatory_string = "TestWithCustom";
  expected.mandatory_int = 600;
  expected.data = CustomStruct::optional_custom{"Custom info", "Detailed description"s};

  auto json = R"(
    {
      "mandatory_string": "TestWithCustom",
      "mandatory_int": 600,
      "optional_custom": {
        "info": "Custom info",
        "details": "Detailed description"
      }
    }
  )";

  auto got = parse_json<CustomStruct>(json);
  EXPECT_EQ(expected, got);
})

JSON_PARSE_TEST(TestOneOfFieldInvalidData, {
  auto json = R"(
    {
      "mandatory_string": "InvalidData",
      "mandatory_int": "not_an_integer",
      "optional_bool": "not_a_boolean"
    }
  )";
  EXPECT_THROW(parse_json<CustomStruct>(json), std::exception);
})

JSON_PARSE_TEST(TestOneOfFieldMissingRequiredFields, {
  auto json = R"(
    {
      "optional_bool": true
    }
  )";
  EXPECT_THROW(parse_json<CustomStruct>(json), std::exception);
})
