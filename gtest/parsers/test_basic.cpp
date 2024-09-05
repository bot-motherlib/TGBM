#include <parsers/fixtures.hpp>

namespace json_parser::test_basic {

struct SomeStruct {
  int a;
  tgbm::api::Double b;
  std::string c;

  static bool consteval is_optional_field(std::string_view name) {
    return tgbm::utils::string_switch<std::optional<bool>>(name)
        .case_("a", true)
        .case_("b", true)
        .case_("c", true)
        .or_default(std::nullopt)
        .value();
  }

  constexpr std::strong_ordering operator<=>(const SomeStruct&) const = default;
};

static_assert(tgbm::common_api_type<SomeStruct>);

JSON_PARSE_TEST(Bool, {
  auto expected = true;
  auto got = parse_json<bool>("true");
  EXPECT_EQ(expected, got);
})

JSON_PARSE_TEST(Int, {
  auto expected = 1231;
  auto got = parse_json<int>("1231");
  EXPECT_EQ(expected, got);
})

JSON_PARSE_TEST(Float, {
  auto expected = 1.0;
  auto got = parse_json<float>("1.0");
  EXPECT_EQ(expected, got);
})

JSON_PARSE_TEST(OptFloat, {
  tgbm::api::optional<float> expected = 1.0;
  auto got = parse_json<tgbm::api::optional<float>>("1.0");
  EXPECT_EQ(expected, got);
})

JSON_PARSE_TEST(BoxFloat, {
  tgbm::box<float> expected = 1.0;
  auto got = parse_json<tgbm::box<float>>("1.0");
  EXPECT_EQ(expected, got);
})

JSON_PARSE_TEST(String, {
  std::string expected = "Some string";
  auto got = parse_json<std::string>(R"(
            "Some string"
        )");
  EXPECT_EQ(expected, got);
})

JSON_PARSE_TEST(BasicAggregate, {
  SomeStruct expected = {.a = 1, .b = 42.5, .c = "some kind"};
  auto got = parse_json<SomeStruct>(R"(
            {
                "a" : 1,
                "b" : 42.5,
                "c" : "some kind"
            }
        )");
  EXPECT_EQ(expected, got);
})

}  // namespace json_parser::test_basic
