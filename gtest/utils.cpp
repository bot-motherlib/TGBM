#include <gtest/gtest.h>

#include <tgbm/utils/string_switch.hpp>
#include <tgbm/utils/box.hpp>
#include <tgbm/utils/url_encoded.hpp>

#include <fmt/core.h>
#include <fmt/ranges.h>

consteval int sstest1() {
  std::string_view str = "hello";
  return tgbm::string_switch<int>(str).case_("hi", 1).cases("hello", "world", 15);
}
static_assert(sstest1() == 15);

consteval int sstest2() {
  std::string_view str = "hello";
  return tgbm::string_switch<int>(str).case_("hi", 1).cases("hello", 15);
}
static_assert(sstest2() == 15);

consteval int sstest3() {
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

struct strange_type {
  int* ptr = nullptr;

  strange_type() noexcept(false) {
  }

  strange_type(int* p) noexcept(false) : ptr(p) {
  }

  strange_type(strange_type&& o) : ptr(o.ptr) {
    o.ptr = nullptr;
  }

  strange_type& operator=(strange_type&& o) {
    std::swap(ptr, o.ptr);
    return *this;
  }

  ~strange_type() {
    if (ptr)
      ++*ptr;
  }
};

struct strange_type1 : strange_type {
  strange_type1() noexcept : strange_type() {
  }
  using strange_type::strange_type;
};

struct strange_type2 : strange_type {
  strange_type2() = default;
  strange_type2(int* ptr) noexcept : strange_type(ptr) {
  }
};

TEST(utils, box2) {
  tgbm::box<strange_type> b;
  int count = 0;
  b.emplace(&count);
  EXPECT_TRUE(b);

  int count2 = 0;
  b.emplace(&count2);

  EXPECT_EQ(count, 1);

  tgbm::box<strange_type1> b1;
  b1.emplace(&count);
  EXPECT_TRUE(b1);

  b1.emplace(&count2);

  EXPECT_EQ(count, 2);

  tgbm::box<strange_type2> b2;
  b2.emplace(&count);
  EXPECT_TRUE(b2);

  b2.emplace(&count2);

  EXPECT_EQ(count, 3);
}
