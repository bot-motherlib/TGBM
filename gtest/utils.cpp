#include <gtest/gtest.h>

#include <tgbm/utils/string_switch.hpp>
#include <tgbm/utils/box.hpp>
#include <tgbm/utils/url_encoded.hpp>

#include <fmt/core.h>
#include <fmt/ranges.h>

constexpr int sstest1() {
  std::string_view str = "hello";
  return tgbm::string_switch<int>(str).case_("hi", 1).cases("hello", "world", 15);
}
static_assert(sstest1() == 15);

constexpr int sstest2() {
  std::string_view str = "hello";
  return tgbm::string_switch<int>(str).case_("hi", 1).cases("hello", 15);
}
static_assert(sstest2() == 15);

constexpr int sstest3() {
  std::string_view str = "hello";
  return tgbm::string_switch<int>(str).case_("hi", 1).cases("world", 15).or_default(15);
}
static_assert(sstest3() == 15);

TEST(utils, box) {
  tgbm::box b(5);
  static_assert(std::is_same_v<decltype(b), tgbm::box<int>>);
  b.emplace(15);
  EXPECT_FALSE(!b || *b != 15);
  b.reset();
  EXPECT_FALSE(b);
  tgbm::box b2 = std::string("hello world");
  EXPECT_FALSE(!b2 || *b2 != "hello world");
  b2.reset();
  EXPECT_FALSE(b2);

  using tgbm::is_lowercase;
  static_assert(is_lowercase("hello"));
  static_assert(is_lowercase(""));
  static_assert(!is_lowercase("A"));
  static_assert(!is_lowercase("aaAbb"));
  static_assert(is_lowercase("f__rew../wwr44"));
  static_assert(!is_lowercase("rttr___trertg;..;A"));
}
