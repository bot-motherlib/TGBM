
#include "tgbm/tools/StringTools.h"

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

int main() {
  return 0;
}
