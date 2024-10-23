#include "tgbm/api/Integer.hpp"
#include "tgbm/api/common.hpp"
#include "tgbm/api/optional.hpp"
#include "tgbm/tools/box_union.hpp"
#include "tgbm/api/const_string.hpp"

#include <vector>
#include <memory>

#define TEST(name) static void test_##name()
#define error_if(...)  \
  if (!!(__VA_ARGS__)) \
  exit(__LINE__)

struct empty_test_type {
  bool operator==(const empty_test_type&) const = default;
};

#ifndef _WIN32
static_assert(sizeof(tgbm::api::optional<empty_test_type>) == 1);
#endif

static_assert(sizeof(tgbm::api::optional<tgbm::const_string>) == sizeof(void*));
static_assert(sizeof(tgbm::api::optional<bool>) == sizeof(bool));
static_assert(sizeof(tgbm::api::optional<tgbm::api::Integer>) == sizeof(tgbm::api::Integer));

template <typename T>
constexpr bool opttest() {
  tgbm::api::optional<T> opt = std::nullopt;
  if (opt)
    throw;
  if (opt != opt)
    throw;
  opt.reset();
  if (opt)
    throw;
  opt.emplace();
  if (!opt)
    throw;
  opt.swap(opt);
  if (!opt)
    throw;
  if (opt != opt)
    throw;
  T& x = opt.value();
  return true;
}

TEST(optional) {
  static_assert(opttest<empty_test_type>());
  static_assert(opttest<tgbm::api::Integer>());
  static_assert(std::is_trivially_copyable_v<tgbm::api::optional<tgbm::api::Integer>>);
  static_assert(opttest<tgbm::api::String>());
  static_assert(opttest<int>());
  error_if(!opttest<tgbm::api::Boolean>());
}

template <int>
struct alignas(8) E {};

static_assert(tgbm::is_unique_types<int>());
static_assert(tgbm::is_unique_types<>());
static_assert(tgbm::is_unique_types<int, int&>());
static_assert(!tgbm::is_unique_types<int, int>());
static_assert(!tgbm::is_unique_types<int, float, void, int, void>());
static_assert(!tgbm::is_unique_types<int, int, void, void>());
static_assert(tgbm::is_unique_types<int, void, double>());

static_assert(tgbm::log2_constexpr(1) == 0);
static_assert(tgbm::log2_constexpr(2) == 1);
static_assert(tgbm::log2_constexpr(3) == 1);
static_assert(tgbm::log2_constexpr(4) == 2);
static_assert(tgbm::log2_constexpr(7) == 2);
static_assert(tgbm::log2_constexpr(16) == 4);

template <typename... Types>
void box_union_test(Types... vars) {
  auto test1_init = [&]<typename T>(const T& var) {
    tgbm::box_union<Types...> x(var);
    error_if(x.is_null());
    error_if(tgbm::get_if<T>(&x) == nullptr);
    error_if(x.index() != tgbm::find_first<T, Types...>());
  };
  (test1_init(vars), ...);
  auto test1_init_nullptr = []<typename T>(const T&) {
    if constexpr (sizeof...(Types) != 0) {
      tgbm::box_union<Types...> x = tgbm::box_union<Types...>::from_ptr((T*)nullptr);
      error_if(x.index() != sizeof...(Types));
      error_if(tgbm::get_if<T>(&x) != nullptr);
      error_if(!x.is_null());
    }
  };
  (test1_init_nullptr(vars), ...);

  tgbm::box_union<Types...> u;
  error_if(u);
  error_if(u != nullptr);
  error_if(u.index() != sizeof...(Types));
  error_if(u != u);
  u.swap(u);
  error_if(u != u);
  u = u;
  error_if(u != u);
  u = std::move(u);
  error_if(u != u);
  u.visit(tgbm::matcher{[](tgbm::nothing_t) {}, [](auto&) { error_if(true); }});
  auto test_var = [&]<typename T>(T var) {
    u = var;
    error_if(u != u);
    u.swap(u);
    error_if(u != u);
    u = u;
    auto ucopy = u;
    error_if(ucopy != u);
    auto ucopy2 = std::move(ucopy);
    error_if(ucopy2 != u);
    error_if(!ucopy.is_null());
    error_if(ucopy);
    error_if(!ucopy2);
    error_if(u != u);
    u = std::move(u);
    error_if(u != u);
    u.visit(tgbm::matcher([&](T& p) { error_if(p != var); }, [](auto&&) { error_if(true); }));
    // same with move assignment
    u = std::move(var);
    error_if(u != u);
    u.swap(u);
    error_if(u != u);
    u = u;
    error_if(u != u);
    u = std::move(u);
    error_if(u != u);
    u.visit(tgbm::matcher([&](T& p) {}, [](auto&&) { error_if(true); }));
  };
  (test_var(std::move(vars)), ...);
}

TEST(box_union_all) {
  //  3
  box_union_test(int(4), float(4.f), std::string("hello world"));
  // 17 (big)
  bool bbb = false;
  int i = 42;
  box_union_test(4, float(4.f), unsigned(i), std::string("hello world"), true, 'c', double(4.),
                 std::vector<int>{1, 2, 3},
                 std::vector<std::string>{std::string("hello"), std::string("world")}, std::shared_ptr<int>{},
                 std::shared_ptr<std::string>(new std::string("u")),
                 std::vector<std::vector<int>>{{1, 2}, {3, 4}}, (void*)nullptr, "char*", &bbb, &i,
                 std::vector<int*>{&i});
}

TEST(box_union_basic) {
  tgbm::box_union<int, float, std::string> u;
  u = "hello world";
  error_if(u.is_null());
  error_if(u.index() != 2);
  error_if(u != std::string("hello world"));
}

TEST(box_union_from_box) {
  tgbm::box_union<int, std::string, tgbm::box<int>> u;
  tgbm::box b(std::string("hello world"));
  u = std::move(b);
  error_if(b);
  error_if(!u);
  error_if(u != "hello world");
  u = 5;
  // selects another operator= (for box<int>)
  u = tgbm::box<int>(5);
  tgbm::box_union<int, std::string, tgbm::box<int>> u2(tgbm::box(std::string("hello")));
  error_if(!u2);
  error_if("hello" != u2);
}

TEST(box_union_release) {
  tgbm::box_union<E<1>, int, E<0>> u3(42);
  error_if(u3 != 42);
  delete (int*)u3.release().get_ptr();  // check release

  tgbm::box_union<E<1>, int, E<0>> u4(42);
  auto u4_copy = std::move(u4);
  error_if(u4);
  delete (int*)u4_copy.release().get_ptr();
}
TEST(box_union_compare) {
  tgbm::box_union<int, std::string> us;
  tgbm::box_union<int, std::string> us2;
  tgbm::box_union<E<1>, int, E<0>> u3 = 42;
  error_if(u3 != 42);
  error_if(41 >= u3);
  error_if(us2 == 0);
  error_if(us2 == "");
  error_if(us != us2);
  error_if((us <=> us2) != std::strong_ordering::equal);
  error_if("hello" != us.emplace<std::string>("hello"));
  error_if(us != decltype(us)("hello"));
  error_if(us == us2);
  error_if((us <=> us2) != std::strong_ordering::greater);
  error_if((us2 <=> us) != std::strong_ordering::less);
  error_if(us.index() != 1);  // index of string
  us2 = us;
  error_if(us != us2);
}

TEST(boolean) {
  tgbm::api::optional<tgbm::api::Boolean> b;
  bool& y = b.emplace();
  error_if(y);
  error_if(!b);
  bool& x = *b;
  error_if(x);
  b.reset();
  error_if(b);
  b = true;
  error_if(!b || !*b);
}

TEST(const_string) {
  using tgbm::const_string;

  // empty

  {
    const_string s1;
    error_if(s1.data() != (char*)&s1);
    error_if(s1.size() != 0);
    error_if(!s1.empty());
  }

  {
    const_string s(5, 'a');
    error_if(s != "aaaaa");
  }

  {
    const_string s(7, 'a');
    error_if(s != "aaaaaaa");
  }

  // small

  const_string small = "Test";
  error_if(small.data() != (char*)&small);
  error_if(small.size() != 4);
  error_if(small != "Test");

  // big

  std::string long_str(100, 'a');
  const_string bigstr = long_str;
  error_if(bigstr != const_string(100, 'a'));
  error_if(bigstr.size() != 100);
  error_if(bigstr != long_str);

  // self assign big

  bigstr = bigstr;
  error_if(bigstr != const_string(100, 'a'));
  bigstr = std::move(bigstr);
  error_if(bigstr != const_string(100, 'a'));

  // copy small

  const_string s4 = small;
  error_if(s4.size() != small.size());
  error_if(s4.str() != small.str());

  // copy big

  const_string s5 = bigstr;
  error_if(s5.size() != bigstr.size());
  error_if(s5.str() != bigstr.str());

  // assign small

  const_string s6;
  s6 = small;
  error_if(s6.size() != small.size());
  error_if(s6.str() != small.str());

  // assign big

  const_string s7;
  s7 = bigstr;
  error_if(s7.size() != bigstr.size());
  error_if(s7.str() != bigstr.str());

  // move small

  const_string s8 = std::move(small);
  error_if(s8.size() != 4);
  error_if(s8.str() != "Test");
  error_if(small.size() != 0);

  // move big

  const_string s9 = std::move(bigstr);
  error_if(s9.size() != 100);
  error_if(s9.str() != long_str);
  error_if(bigstr.size() != 0);
  error_if(!bigstr.empty());
  error_if(bigstr != const_string{});

  // cmp equal

  const_string s10 = "Hello";
  const_string s11 = "Hello";
  const_string s12 = "World";
  error_if(!(s10 == s11));
  error_if(s10 == s12);

  // swap small

  const_string s13 = "First";
  const_string s14 = "Second";
  s13.swap(s14);
  error_if(s13.str() != "Second");
  error_if(s14.str() != "First");

  // reset

  const_string s15 = "To be cleared";
  s15.reset();
  error_if(s15.size() != 0);
  error_if(!s15.str().empty());

  const_string s16 = "Move this";
  const_string s17;
  s17 = std::move(s16);
  error_if(s17.str() != "Move this");
  error_if(s16.size() != 0);

  const_string s18 = "";
  error_if(s18.size() != 0);
  error_if(!s18.str().empty());

  // assign string view

  std::string_view sv = "String view";
  const_string s19 = sv;
  error_if(s19.size() != sv.size());
  error_if(s19.str() != sv);

  const_string s20 = "Initial";
  s20 = "First";
  s20 = "Second";
  s20 = "Third";
  error_if(s20 != "Third");

  // self assign small

  s20 = s20;
  error_if(s20 != "Third");

  // double reset

  const_string s21;
  s21.reset();
  s21.reset();
  error_if(s21.size() != 0);

  const_string s22 = "Permanent";
  const_string s23 = std::move(s22);
  error_if(s23 != "Permanent");
  error_if(s22.size() != 0);
  error_if(!s22.empty());

  const_string s24;
  const_string s25;
  error_if(s24 != s25);

  // cmp big and small

  const_string s26 = "Short";
  const_string s27(100, 'S');
  error_if(s26 == s27);

  const_string s29 = "1234567";
  error_if(s29.size() != 7);
  error_if(s29 != "1234567");
}

TEST(optional_const_string) {
  using opt_t = tgbm::api::optional<tgbm::const_string>;
  opt_t opt;
  error_if(opt);
  error_if(opt.has_value());
  error_if(opt != std::nullopt);
  try {
    opt.value();
    error_if(true);
  } catch (...) {
  }
  opt.emplace(10, 'c');
  error_if(opt != tgbm::const_string(10, 'c'));

  error_if(!opt);
  error_if(!opt.has_value());
  error_if(opt->empty());
  opt.emplace("abc");
  error_if(opt.value() != "abc");
  error_if(opt.value_or("hello") != "abc");
  error_if(opt->size() != 3);
  opt.reset();
  error_if(opt != std::nullopt);
  error_if(opt.value_or("hello") != "hello");
}

int main() {
  test_optional_const_string();
  test_const_string();
  test_optional();
  test_box_union_release();
  test_box_union_compare();
  test_box_union_from_box();
  test_box_union_basic();
  test_box_union_all();
  test_boolean();

  static_assert(sizeof(tgbm::box_union<int, float, std::string>) <= sizeof(void*));
  static_assert(sizeof(tgbm::box_union<E<0>, E<1>, E<2>, E<3>, E<4>, E<5>, E<6>, E<7>, E<8>, E<9>, E<10>,
                                       E<11>, E<12>, E<13>, E<14>, E<15>>) <= sizeof(void*) * 2);
}
