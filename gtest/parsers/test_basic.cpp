#include <parsers/fixtures.hpp>

namespace json_parser::test_basic {

struct SomeStruct {
  tgbm::api::optional<int> a;
  tgbm::api::optional<tgbm::api::Double> b;
  tgbm::api::optional<std::string> c;

  static consteval bool is_mandatory_field(std::string_view name) {
    return false;
  }

  std::strong_ordering operator<=>(const SomeStruct&) const = default;
};

static_assert(tgbm::common_api_type<SomeStruct>);

JSON_PARSE_TEST(Bool, bool) {
  auto expected = true;
  auto got = parse_json("true");
  EXPECT_EQ(expected, got);
}

JSON_PARSE_TEST(UIntPositive, unsigned int) {
  auto expected = 1;
  auto got = parse_json("1");
  EXPECT_EQ(expected, got);
}

JSON_PARSE_TEST(UIntZero, unsigned int) {
  auto expected = 0;
  auto got = parse_json("0");
  EXPECT_EQ(expected, got);
}

JSON_PARSE_TEST(IntPositive, int) {
  auto expected = 1231;
  auto got = parse_json("1231");
  EXPECT_EQ(expected, got);
}

JSON_PARSE_TEST(IntZero, int) {
  auto expected = 0;
  auto got = parse_json("0");
  EXPECT_EQ(expected, got);
}

JSON_PARSE_TEST(IntNegative, int) {
  auto expected = -1;
  auto got = parse_json("-1");
  EXPECT_EQ(expected, got);
}

JSON_PARSE_TEST(Float, float) {
  auto expected = 1.0;
  auto got = parse_json("1.0");
  EXPECT_EQ(expected, got);
}

JSON_PARSE_TEST(OptFloat, tgbm::api::optional<float>) {
  tgbm::api::optional<float> expected = 1.0;
  auto got = parse_json("1.0");
  EXPECT_EQ(expected, got);
}

JSON_PARSE_TEST(BoxFloat, tgbm::box<float>) {
  tgbm::box<float> expected = 1.0;
  auto got = parse_json("1.0");
  EXPECT_EQ(expected, got);
}

JSON_PARSE_TEST(String, std::string) {
  std::string expected = "Some string";
  auto got = parse_json(R"(
            "Some string"
        )");
  EXPECT_EQ(expected, got);
}

JSON_PARSE_TEST(BasicAggregate, SomeStruct) {
  SomeStruct expected = {.a = 1, .b = 42.5, .c = "some kind"};
  auto got = parse_json(R"(
            {
                "a" : 1,
                "b" : 42.5,
                "c" : "some kind"
            }
        )");
  EXPECT_EQ(expected, got);
}

}  // namespace json_parser::test_basic
