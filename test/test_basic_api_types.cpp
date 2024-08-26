#include "tgbm/api_types/Integer.h"
#include "tgbm/api_types/common.h"
#include "tgbm/api_types/optional.h"
#include "tgbm/tools/box_union.hpp"

#include <vector>
#include <memory>

template <typename T>
constexpr int opttest() {
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
  return 1;
}
static_assert(opttest<tgbm::api::Integer>());
static_assert(opttest<tgbm::api::String>());
static_assert(opttest<int>());

consteval int test1() {
  tgbm::api::Integer i = 1;
  if (i.value != 1)
    throw;
  return 0;
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

#define error_if(...)  \
  if (!!(__VA_ARGS__)) \
  exit(__LINE__)
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
      error_if(x.index() != tgbm::find_first<T, Types...>());
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
  u.visit_ptr(tgbm::matcher{[](tgbm::nothing_t) {}, [](auto*) { error_if(true); }});
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
    u.visit_ptr(tgbm::matcher([&](T* p) { error_if(*p != var); }, [](auto&&) { error_if(true); }));
    // same with move assignment
    u = std::move(var);
    error_if(u != u);
    u.swap(u);
    error_if(u != u);
    u = u;
    error_if(u != u);
    u = std::move(u);
    error_if(u != u);
    u.visit_ptr(tgbm::matcher([&](T* p) {}, [](auto&&) { error_if(true); }));
  };
  (test_var(std::move(vars)), ...);
}

void box_union_test_basic() {
  tgbm::box_union<int, float, std::string> u;
  u = "hello world";
  error_if(u.is_null());
  error_if(u.index() != 2);
  error_if(u != std::string("hello world"));
}

void box_union_test_box() {
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

int main() {
  box_union_test_box();
  box_union_test_basic();
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

  static_assert(sizeof(tgbm::box_union<int, float, std::string>) <= sizeof(void*));
  static_assert(sizeof(tgbm::box_union<E<0>, E<1>, E<2>, E<3>, E<4>, E<5>, E<6>, E<7>, E<8>, E<9>, E<10>,
                                       E<11>, E<12>, E<13>, E<14>, E<15>>) <= sizeof(void*) * 2);
  tgbm::box_union<int, std::string> us;
  tgbm::box_union<int, std::string> us2;
  tgbm::box_union<E<1>, int, E<0>> u3(42);  // 1 comparable type + 1 incomparable
  error_if(u3 != 42);
  delete (int*)u3.release().get_ptr();  // check release

  tgbm::box_union<E<1>, int, E<0>> u4(42);  // 1 comparable type + 1 incomparable
  auto u4_copy = std::move(u4);
  error_if(u4);
  delete (int*)u4_copy.release().get_ptr();

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
  // cannot be constexpr evaluated because of union
  if (!opttest<tgbm::api::Boolean>())
    throw;
  tgbm::api::optional<tgbm::api::Boolean> b;
  bool& y = b.emplace();
  if (y)
    throw;
  if (!b)
    throw;
  bool& x = *b;
  if (x)
    throw;
  b.reset();
  if (b)
    throw;
  b = true;
  if (!b || !*b)
    throw;
}
