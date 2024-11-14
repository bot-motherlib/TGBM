
#include "tgbm/tools/StringTools.h"
#include "tgbm/tools/box.hpp"

#include <random>

#include <fmt/core.h>
#include <fmt/ranges.h>

#include "tgbm/jsons/generator_parser/stack_memory_resource.hpp"

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

static void test_allocator(size_t seed) {
  alignas(dd::coroframe_align()) unsigned char bytes[1];
  tgbm::stack_resource resource(bytes);

  struct chunk_data {
    void* ptr;
    size_t size;
    tgbm::byte_t filling;

    chunk_data(void* p, size_t sz, tgbm::byte_t fill) noexcept : ptr(p), size(sz), filling(fill) {
      std::span data((tgbm::byte_t*)p, sz);
      std::fill(data.begin(), data.end(), fill);
    }

    void invariant_check() {
      tgbm::byte_t* b = (tgbm::byte_t*)ptr;
      for (tgbm::byte_t byte : std::span(b, size)) {
        if (byte != filling)
          throw std::runtime_error("invariant failed");
      }
    }
  };

  std::vector<chunk_data> chunks;
  std::mt19937 gen(seed);
  for (int i = 0; i < 1000; ++i) {
    if (chunks.size() && std::bernoulli_distribution(0.5)(gen)) {
      // fmt::println("MINUS({})", chunks.back().size);
      chunks.back().invariant_check();
      resource.deallocate(chunks.back().ptr, chunks.back().size);
      chunks.pop_back();
    } else {
      size_t len = std::uniform_int_distribution<size_t>(0, 200)(gen);
      // fmt::println("PLUS({})", len);
      chunks.emplace_back(resource.allocate(len), len, std::uniform_int_distribution<int>('A', 'Z')(gen));
      assert(((uintptr_t)chunks.back().ptr % 16) == 0);
    }
  }
}

static void test_allocator2() {
  alignas(dd::coroframe_align()) unsigned char bytes[1];
  tgbm::stack_resource r(bytes);
  std::vector<void*> ptrs;
#define PLUS(N)                  \
  ptrs.push_back(r.allocate(N)); \
  assert(((uintptr_t)ptrs.back() % 16) == 0)
#define MINUS(N)                \
  r.deallocate(ptrs.back(), N); \
  ptrs.pop_back()

  PLUS(3);
  PLUS(85);
  PLUS(33);
  PLUS(55);
  MINUS(55);
  MINUS(33);
  MINUS(85);
  PLUS(194);
  MINUS(194);
  MINUS(3);
}

int main() {
  test_allocator2();
  if (!boxtest1())
    return -1;
  int min_fail_sec = INT_MAX;
  for (int seed = 0; seed < 10000; ++seed)
    test_allocator(seed);
  return 0;
}
