
#include "tgbm/tools/StringTools.h"
#include "tgbm/tools/box.hpp"

consteval int sstest1() {
  std::string_view str = "hello";
  return tgbm::utils::string_switch<int>(str).case_("hi", 1).cases("hello", "world", 15);
}
static_assert(sstest1() == 15);

consteval int sstest2() {
  std::string_view str = "hello";
  return tgbm::utils::string_switch<int>(str).case_("hi", 1).cases("hello", 15);
}
static_assert(sstest2() == 15);

consteval int sstest3() {
  std::string_view str = "hello";
  return tgbm::utils::string_switch<int>(str).case_("hi", 1).cases("world", 15).or_default(15);
}
static_assert(sstest3() == 15);

consteval bool boxtest1() {
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
static_assert(boxtest1());

int main() {
  return 0;
}
