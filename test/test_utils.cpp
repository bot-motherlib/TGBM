
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

bool boxtest1() {
  tgbm::box b(5);
  static_assert(std::is_same_v<decltype(b), tgbm::box<int>>);
  b.emplace(15);
  if (!b || *b != 15)
    throw 42;
  b.reset();
  if (b)
    throw 42;
  tgbm::box b2 = std::string("hello world");
  if (!b2 || *b2 != "hello world")
    throw 42;
  b2.reset();
  if (b2)
    throw 42;
  return true;
}

using tgbm::is_lowercase;
static_assert(is_lowercase("hello"));
static_assert(is_lowercase(""));
static_assert(!is_lowercase("A"));
static_assert(!is_lowercase("aaAbb"));
static_assert(is_lowercase("f__rew../wwr44"));
static_assert(!is_lowercase("rttr___trertg;..;A"));

int main() {
  if (!boxtest1())
    return -1;
  return 0;
}
