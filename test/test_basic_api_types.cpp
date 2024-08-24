#include "tgbm/api_types/Integer.h"
#include "tgbm/api_types/common.h"
#include "tgbm/api_types/optional.h"

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

int main() {
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
