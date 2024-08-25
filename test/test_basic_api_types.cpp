#include "tgbm/api_types/Integer.h"
#include "tgbm/api_types/common.h"
#include "tgbm/api_types/optional.h"
#include "tgbm/api_types/box_union.h"

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
// TODO нормальные тесты, чтобы выглядело ну не так
int test_ptrint_pair() {
  tgbm::noexport::ptrint_pair_for<int, float, std::string> a(nullptr);
  static_assert(decltype(a)::max_int_val == 3);  // 2 bits - 1
  static_assert(tgbm::noexport::ptrint_pair_for<double, std::string>::max_int_val == 7);
  a.init_with_ptr(nullptr);
  if (a.get_ptr() != nullptr)
    return 1;
  if (a.get_int() != 0)
    return 2;
  int x = 42;
  a.set_ptr_and_int(&x, 3);
  int ii = a.get_int();
  if (ii != 3)
    return 4;
  if (a.get_ptr() != &x || a.get_int() != 3)
    return 3;
  return 0;
}
template <int>
struct alignas(8) E {};

struct vtor_t {
  int x = 0;

  void operator()(int* val) {
    x = *val;
  }
  void operator()(auto*) {
  }
};

int test_pointer_union() {
  static_assert(tgbm::pointer_traits_t<E<0>*>::num_low_bits_available);
  tgbm::pointer_union<E<0>, E<1>, E<2>, E<3>, E<4>, E<5>, E<6>, E<7>> l;
  (void)l;  // can contain 8 types
  tgbm::pointer_union<int, float, std::string> u = nullptr;
  static_assert(sizeof(u) == sizeof(void*));
  static_assert(tgbm::noexport::bits_required(3) == 2);
  static_assert(u.indexof<int>() == 0);
  static_assert(u.indexof<float>() == 1);
  static_assert(u.indexof<std::string>() == 2);
  static_assert(u.indexof<void>() == -1);

  if (u)
    return 1;
  int i = 42;
  u = &i;
  if (!u)
    return 2;
  vtor_t vtor;
  u.visit(vtor);
  if (vtor.x != 42)
    return 3;
  u = nullptr;
  if (u)
    return 4;
  std::string str = "hello world";
  auto u2 = decltype(u)(&str);
  if (u2.index() != 2)
    return 7;
  u = u2;
  int ui = u.index();
  if (ui != 2)
    return 6;
  if (!u || !u.get_if<std::string>() || *u.get_if<std::string>() != "hello world")
    return 5;
  return 0;
}

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

int main() {
  if (test_ptrint_pair() != 0)
    return 1;
  if (test_pointer_union() != 0)
    return 1;
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
